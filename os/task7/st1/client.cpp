#include <arpa/inet.h>
#include <cstdio>
#include <iostream>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

constexpr int PORT = 8080;
constexpr int BUFFER_SIZE = 1024;

int main()
{
    // Создание UDP-сокета
    int clientSocket;
    if ((clientSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        std::cerr << "Error creating socket\n";
        return -1;
    }

    // Настройка адреса сервера
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("172.21.122.22");

    while (true)
    {
        // ввод сообщения через консоль
        char message[BUFFER_SIZE];
        std::cout << "Enter message: ";
        std::cin >> message;
        if (strlen(message) == 0)
            continue;

        // Отправка данных на сервер
        ssize_t sendLen = sendto(clientSocket, message, strlen(message), 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
        if (sendLen == -1)
        {
            std::cerr << "Error sending data\n";
            close(clientSocket);
            return -1;
        }
        std::cout << "Client: sent message to server\n";

        // Получение данных от сервера
        char buffer[BUFFER_SIZE];
        socklen_t addrLen = sizeof(serverAddr);
        ssize_t recvLen = recvfrom(clientSocket, buffer, BUFFER_SIZE, 0, nullptr, &addrLen);
        if (recvLen == -1)
        {
            std::cerr << "Error receiving data\n";
            close(clientSocket);
            return -1;
        }
        buffer[recvLen] = '\0';
        std::cout << "Client: received message \"" << buffer << "\"\n";
    }

    close(clientSocket);
    return 0;
}
