#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <string>
#pragma comment(lib, "ws2_32.lib")

void receiveFile(SOCKET socket) {
    char fileName[256];
    recv(socket, fileName, sizeof(fileName), 0);

    std::ofstream file(fileName, std::ios::binary);
    if (!file.is_open()) {
        std::cout << "Failed to create file\n" << std::endl;
        return;
    }

    char buffer[1024];
    int bytesReceived = recv(socket, buffer, sizeof(buffer), 0);

    if (bytesReceived <= 0) {
        return;
    }
    
    file.write(buffer, bytesReceived);
    file.close();

    std::cout << "File received and saved as " << fileName << std::endl;
}

void showFileContent(SOCKET socket) {
    char fileName[256];
    recv(socket, fileName, sizeof(fileName), 0);

    std::string content;

    std::ifstream file(fileName);
    if (!file.is_open()) {
        content = "Failed to open file";
        send(socket, content.c_str(), content.size() + 1, 0);
        return;
    }

    std::string line;
    while (getline(file, line)) {
        content += line + "\n";
    }

    send(socket, content.c_str(), content.size() + 1, 0);

    std::cout << "File contents " << fileName << " sent to client " << std::endl;
}

int main()
{
    WSADATA ws;
    struct sockaddr_in local;
    int clientSocket;
    int serverSocket;
    int rc;

    local.sin_family = AF_INET;
    local.sin_port = htons(1111);
    local.sin_addr.s_addr = htonl(INADDR_ANY);

    if (WSAStartup(0x0202, &ws) != 0)
    {
        printf("Error");
        exit(1);
    }

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        perror("Socket call failed");
        exit(1);
    }

    rc = bind(serverSocket, (struct sockaddr*)&local, sizeof(local));
    if (rc < 0)
    {
        perror("Bind call failure");
        exit(1);
    }

    rc = listen(serverSocket, 5);
    if (rc)
    {
        perror("Listen call failed");
        exit(1);
    }

    clientSocket = accept(serverSocket, NULL, NULL);
    if (clientSocket < 0)
    {
        perror("Accept call failed");
        exit(1);
    }
    else {
        printf("Client connected\n");
    }

    while (true) {
        int receivedChoice;
        recv(clientSocket, (char*)&receivedChoice, sizeof(receivedChoice), 0);

        switch (receivedChoice) {
        case 1:
            receiveFile(clientSocket);
            break;
        case 2:
            showFileContent(clientSocket);
            break;
        case 3: 
            return 0;
        default: 
            printf("Invalid choice\n");
            break;
        }  
    }

    closesocket(clientSocket);
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
