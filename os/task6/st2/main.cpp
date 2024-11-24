
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

const int MAX_ITERATIONS = 100000;

int main()
{
    // Создание канала pipe
    int pipefd[2];
    if (pipe(pipefd) == -1)
    {
        std::cerr << "Failed to create pipe\n";
        return 1;
    }
    const int fdR = pipefd[0]; // для чтения из канала
    const int fdW = pipefd[1]; // для записи в канал

    // Создание дочернего процесса
    const pid_t pid = fork();
    if (pid == -1)
    {
        std::cerr << "Failed to fork\n";
        return 1;
    }

    if (pid == 0) // Дочерний процесс
    {
        close(fdW);

        int prev;
        int i = 0;
        for (; i < MAX_ITERATIONS; ++i)
        {
            int x;
            if (read(fdR, &x, sizeof(x)) == -1)
            {
                std::cerr << "Failed to read from fdR\n";
                break;
            }
            if (x == prev + 1 || i == 0)
                prev = x;
            else
                break;
        }
        std::cout << "Mached numbers: " << i << "\n";

        close(fdR);
        return 0;
    }
    else // Родительский процесс
    {
        close(fdR);

        for (int i = 0; i < MAX_ITERATIONS; ++i)
        {
            if (write(fdW, &i, sizeof(i)) == -1)
            {
                std::cerr << "Failed to write from fdR\n";
                break;
            }
        }

        close(fdW);

        int status;
        wait(&status);
        if (WIFEXITED(status))
            std::cout << "The child process ended with the code: " << WEXITSTATUS(status) << "\n";
        else
            std::cout << "The child process ended abruptly\n";
    }

    return 0;
}
