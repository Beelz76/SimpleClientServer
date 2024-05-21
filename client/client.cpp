#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)

void sendFile(SOCKET socket) {
    std::cout << "Enter the file name: ";
    std::string fileName;
    std::cin >> fileName;

    std::ifstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "File does not exist" << std::endl;
        return;
    }

    send(socket, fileName.c_str(), fileName.size() + 1, 0);

    char buffer[1024];

    while (!file.eof()) {
        file.read(buffer, sizeof(buffer));
        int bytesRead = file.gcount();
        send(socket, buffer, bytesRead, 0);
    }

    file.close();

    std::cout << "File sent successfully" << std::endl;
}

void readFile(SOCKET socket) {
    std::cout << "Enter the file name: ";
    std::string fileName;
    std::cin >> fileName;

    send(socket, fileName.c_str(), fileName.size() + 1, 0);

    char buffer[1024];

    int bytesRead = recv(socket, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        return;
    }

    std::cout << buffer;
}

int main()
{
    WSADATA ws;
    struct sockaddr_in peer;
    int clientSocket;
    int rc;

    peer.sin_family = AF_INET;
    peer.sin_port = htons(1111);
    peer.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (WSAStartup(0x0202, &ws) != 0)
    {
        printf("Error");
        exit(1);
    }

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket == INVALID_SOCKET)
    {
        perror("Socket call failed");
        exit(1);
    }

    rc = connect(clientSocket, (struct sockaddr*)&peer, sizeof(peer));
    if (rc)
    {
        perror("Connect call failed");
        exit(1);
    }
    else {
        printf("Connected to server\n");
    }

    while (true) {
        printf("\nChoose an action:\n");
        printf("1.Send file\n");
        printf("2.Read file\n");
        printf("3.Exit\n");

        int choice;
        std::cin >> choice;

        send(clientSocket, (char*)&choice, sizeof(int), 0);

        switch (choice) {
        case 1:
            sendFile(clientSocket);
            break;
        case 2:
            readFile(clientSocket);
            break;
        case 3:
            printf("Exit the programm");
            return 0;
        default: 
            printf("Invalid choice. Please try again.\n");
            break;
        }
    }

    closesocket(clientSocket);
    WSACleanup();

    return 0;
}

