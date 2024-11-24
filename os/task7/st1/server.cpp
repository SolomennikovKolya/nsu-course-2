
#include <arpa/inet.h>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

const int PORT = 8080;
const int BUFFER_SIZE = 1024;

int createServerSocket()
{
    // сетевой адрес сервера. Расширяет структуру sockaddr, специально для семейства адресов AF_INET (IPv4)
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;         // Домен (семейство протоколов, которое определяет пространство адресов и правила для их использования в сети). Бывает ещё AF_UNIX (AF_LOCAL) - локальные сокеты Unix и другие
    serverAddr.sin_port = htons(PORT);       // Номер порта. htons - для форматирования порта под определённую архетектуру
    serverAddr.sin_addr.s_addr = INADDR_ANY; // У серверного сокета не будет конкретного IP-адреса. Вместо этого, он будет настроен на прослушивание всех доступных сетевых интерфейсов машины. Это означает, что сервер будет принимать соединения, адресованные любому из IP-адресов, назначенных на этом хосте.

    // Создание UDP-сокета
    int serverSocket;
    if ((serverSocket = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
    {
        std::cerr << "Error creating socket\n";
        return -1;
    }

    // Привязка сокета к адресу и порту
    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
    {
        std::cerr << "Error binding socket\n";
        close(serverSocket);
        return -1;
    }

    return serverSocket;
}

void printRecvInfo(const sockaddr_in &clientAddr, const ssize_t recvLen)
{
    // Преобразование IP в строку
    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &(clientAddr.sin_addr), ip, INET_ADDRSTRLEN);

    // Преобразование порта в сетевой порядок байтов
    unsigned short port = ntohs(clientAddr.sin_port);

    std::cout << "Server: received " << recvLen << " bytes from " << ip << " : " << port << "\n";
}

int main()
{
    int serverSocket = createServerSocket();
    if (serverSocket == -1)
    {
        std::cerr << "Error creating server socket\n";
        return 1;
    }

    struct sockaddr_in clientAddr;
    socklen_t addrLen = sizeof(clientAddr);
    char buffer[BUFFER_SIZE]; // буфер для пересылки пакетов

    while (true)
    {
        // Прием данных от клиента
        ssize_t recvLen = recvfrom(serverSocket, buffer, BUFFER_SIZE, 0, (struct sockaddr *)&clientAddr, &addrLen);
        if (recvLen == -1)
        {
            std::cerr << "Error receiving data\n";
            close(serverSocket);
            return 1;
        }
        printRecvInfo(clientAddr, recvLen);

        // Отправка данных обратно клиенту
        ssize_t sendLen = sendto(serverSocket, buffer, recvLen, 0, (struct sockaddr *)&clientAddr, addrLen);
        if (sendLen == -1)
        {
            std::cerr << "Error sending data\n";
            close(serverSocket);
            return 1;
        }
        std::cout << "Server: sent message \"" << buffer << "\" back\n";
    }

    close(serverSocket);
    return 0;
}
