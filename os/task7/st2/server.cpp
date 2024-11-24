
#include <arpa/inet.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

const int BUFFER_SIZE = 1024;
const int MAX_QUEUE_LEN = 5;

const char *SERVER_IP = "127.0.0.1";
const int SERVER_PORT = 8080;

// Создание и настройка сокета для сервера
int createServerSocket()
{
    // Сетевой адрес сервера
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;                   // Домен (семейство протоколов, которое определяет пространство адресов и правила для их использования в сети). Бывает ещё AF_UNIX (AF_LOCAL) - локальные сокеты Unix и другие
    serverAddr.sin_port = htons(SERVER_PORT);          // Номер порта. htons - для форматирования порта под определённую архетектуру
    serverAddr.sin_addr.s_addr = inet_addr(SERVER_IP); // У серверного сокета не будет конкретного IP-адреса. Вместо этого, он будет настроен на прослушивание всех доступных сетевых интерфейсов машины. Это означает, что сервер будет принимать соединения, адресованные любому из IP-адресов, назначенных на этом хосте.

    // Сокет настроенный на использование протоколов IPv4 и TCP
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        std::cerr << "Error creating socket\n";
        return -1;
    }

    // Привязка сокета с адресом
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::cerr << "Error binding socket\n";
        close(serverSocket);
        return -1;
    }

    // Перевод сокета в пассивный режим
    if (listen(serverSocket, MAX_QUEUE_LEN) < 0)
    {
        std::cerr << "Listen error\n";
        close(serverSocket);
        return -1;
    }

    printf("Server is listening on %s : %d\n", SERVER_IP, SERVER_PORT);

    return serverSocket;
}

// Вывод информации о клиенте
void printInfo(const sockaddr_in &clientAddr, const ssize_t readed)
{
    // Преобразование IP в строку
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), ip, INET_ADDRSTRLEN);

    // Преобразование порта в сетевой порядок байтов
    unsigned short port = ntohs(clientAddr.sin_port);

    std::cout << "Server: received " << readed << " bytes from " << ip << " : " << port << "\n";
}

// Общение с клиентом
void handleClient(int clientSocket, struct sockaddr_in &clientAddr)
{
    char buffer[BUFFER_SIZE];
    ssize_t readed;

    while (true)
    {
        // Чтение
        readed = read(clientSocket, buffer, BUFFER_SIZE);
        if (readed <= 0)
            break;
        buffer[readed] = '\0';
        printInfo(clientAddr, readed);

        // Эхо-ответ
        write(clientSocket, buffer, readed);
        std::cout << "Server: sent message \"" << buffer << "\" back\n";
    }

    close(clientSocket);
    exit(0);
}

int main(int argc, char *argv[])
{
    int serverSocket = createServerSocket();
    if (serverSocket == -1)
    {
        std::cerr << "Error creating server socket\n";
        return 1;
    }

    // Сокет для соединения с клиентом
    int clientSocket;
    struct sockaddr_in clientAddr;
    socklen_t clientLen = sizeof(clientAddr);

    while (1)
    {
        clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientLen);
        if (clientSocket < 0)
        {
            std::cerr << "Accept failed\n";
            continue;
        }

        pid_t pid = fork();
        if (pid == -1)
        {
            std::cerr << "Fork error\n";
            close(clientSocket);
            continue;
        }

        if (pid == 0)
        {
            // Дочерний процесс
            close(serverSocket);
            handleClient(clientSocket, clientAddr);
        }
        else if (pid > 0)
        {
            // Родительский процесс
            close(clientSocket);
            while (waitpid(-1, NULL, WNOHANG) > 0)
                ; // Убираем зомби-процессы (не блокирует выполнение основного процесса сервера)
        }
    }

    close(serverSocket);
    return 0;
}
