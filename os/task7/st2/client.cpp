
#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

const int BUFFER_SIZE = 1024;

const char *SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 8080;

int main(int argc, char *argv[])
{
    // Сокет клиента
    int clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0)
    {
        std::cerr << "Error creating socket\n";
        return 1;
    }

    // Адрес сервера
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    server_addr.sin_port = htons(SERVER_PORT);

    // Установка соединения
    if (connect(clientSocket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("Connect failed");
        close(clientSocket);
        return 1;
    }
    printf("Connected to server %s : %d\n", SERVER_IP, SERVER_PORT);

    // Переменные
    char message[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    while (true)
    {
        // Ввод сообщения из консоли
        std::cout << "Enter message: ";
        std::cin >> message;
        if (strlen(message) == 0)
            continue;

        // Отправка этого сообщения серверу
        write(clientSocket, message, strlen(message));
        std::cout << "Client: sent message to server\n";

        // Получение ответа
        int readed = read(clientSocket, buffer, BUFFER_SIZE);
        buffer[readed] = '\0';
        std::cout << "Client: received message \"" << buffer << "\"\n";
    }

    close(clientSocket);
    return 0;
}
