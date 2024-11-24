
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int globalVar = 1;

int main()
{
    pid_t pid = getpid();
    std::cout << "PID: " << pid << "\n";

    int localVar = 2;
    std::cout << "globalVar: " << &globalVar << "\t" << globalVar << "\n";
    std::cout << "localVar:  " << &localVar << "\t" << localVar << "\n";
    std::cout << "\n";

    pid_t relativePid = fork(); // создаёт копию родительского процесса (копируется адресное пространство)
    if (relativePid < 0)
    {
        perror("fork");
        return 1;
    }

    if (relativePid == 0) // дочерний процесс
    {
        std::cout << "child:\n";
        std::cout << "pid: " << getpid() << ", parentPid: " << relativePid << "\n";

        std::cout << "globalVar from child: " << &globalVar << "\t" << globalVar << "\n";
        std::cout << "localVar from child:  " << &localVar << "\t" << localVar << "\n";
        globalVar = 3;
        localVar = 4;
        std::cout << "globalVar from child: " << &globalVar << "\t" << globalVar << "\n";
        std::cout << "localVar from child:  " << &localVar << "\t" << localVar << "\n";
        std::cout << "\n";

        sleep(100);
        return 123;
    }
    else // родительский процесс
    {
        return 0;
        sleep(1);
        std::cout << "parent:\n";
        std::cout << "globalVar from child: " << &globalVar << "\t" << globalVar << "\n";
        std::cout << "localVar from child:  " << &localVar << "\t" << localVar << "\n";
        std::cout << "\n";

        int status;
        wait(&status);

        if (WIFEXITED(status))
        {
            // Дочерний процесс завершился нормально
            std::cout << "The child process ended with the code: " << WEXITSTATUS(status) << "\n";
        }
        else
        {
            // Дочерний процесс завершился аварийно
            std::cout << "The child process ended abruptly\n";
        }
    }

    return 0;
}