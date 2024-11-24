
#include <fcntl.h>
#include <iostream>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>

const int MAX_ITERATIONS = 100000;

int main()
{
    const long pageSize = sysconf(_SC_PAGESIZE); // Размер страницы (4096 bytes)
    const int fd = open("sharedFile", O_CREAT | O_RDWR | O_TRUNC, S_IRUSR | S_IWUSR);
    if (fd == -1)
    {
        std::cerr << "Failed to open sharedFile.txt\n";
        return 1;
    }
    if (ftruncate(fd, pageSize) == -1)
    {
        std::cerr << "Failed to ftruncate\n";
        close(fd);
        return 1;
    }

    // Отображение файла в память
    int *region = (int *)mmap(NULL, pageSize, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    const int regionSize = pageSize / sizeof(int);
    if (region == MAP_FAILED)
    {
        std::cerr << "Failed to mmap\n";
        close(fd);
        return 1;
    }
    if (close(fd) == -1)
    {
        std::cerr << "Failed to close fd\n";
        return 1;
    }

    // Создание дочернего процесса
    const pid_t pid = fork();
    if (pid == -1)
    {
        std::cerr << "Failed to fork\n";
        return 1;
    }

    if (pid == 0) // Дочерний процесс
    {
        int prev = region[0] - 1;
        int i = 0;
        for (; i < MAX_ITERATIONS; ++i)
        {
            if (region[i % regionSize] == prev + 1)
                prev++;
            else
                break;
            // std::cout << region[i % regionSize] << " ";
        }
        std::cout << "Mached numbers: " << i << "\n";
        return 0;
    }
    else // Родительский процесс
    {
        for (int i = 0; i < MAX_ITERATIONS; ++i)
            region[i % regionSize] = i;

        int status;
        wait(&status);
        if (WIFEXITED(status))
            std::cout << "The child process ended with the code: " << WEXITSTATUS(status) << "\n";
        else
            std::cout << "The child process ended abruptly\n";
    }

    // Освобождение региона
    if (munmap(region, pageSize) == -1)
    {
        std::cerr << "Failed to munmap\n";
        return 1;
    }

    return 0;
}
