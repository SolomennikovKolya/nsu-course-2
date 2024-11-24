
#include <iostream>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main()
{
    pid_t pid1 = getpid();
    std::cout << "pid1: " << pid1 << "\n";

    pid_t pid2 = fork();
    if (pid2 < 0)
    {
        perror("fork");
        return 1;
    }

    if (pid2 != 0)
    {
        // процесс 1

        std::cout << "pid2: " << pid2 << "\n";

        sleep(100);
        int status;
        wait(&status);

        if (WIFEXITED(status))
            std::cout << "The child process ended with the code: " << WEXITSTATUS(status) << "\n";
        else
            std::cout << "The child process ended abruptly\n";
    }
    else
    {
        // процесс 2

        pid_t pid3 = fork();
        if (pid3 < 0)
        {
            perror("fork");
            return 1;
        }

        if (pid3 != 0)
        {
            // процесс 2

            std::cout << "pid3: " << pid3 << "\n";
            return 0; // не ждёт завершения процесса 3
        }
        else
        {
            // процесс 3

            sleep(100);
            return 0;
        }
    }
    return 0;
}

// если родительский процесс завершится раньше дочернего, то дочерний процесс станет сиротой и подхватится процессом init

/*
uikvel@WORKBOOK:~/workdir/os_tasks/task5/st1$ ps -f 339109
UID          PID    PPID  C STIME TTY      STAT   TIME CMD
uikvel    339109    1630  0 17:46 pts/4    S+     0:00 ./a.out

uikvel@WORKBOOK:~/workdir/os_tasks/task5/st1$ ps -f 339110
UID          PID    PPID  C STIME TTY      STAT   TIME CMD
uikvel    339110  339109  0 17:46 pts/4    Z+     0:00 [a.out] <defunct>

uikvel@WORKBOOK:~/workdir/os_tasks/task5/st1$ ps -f 339111
UID          PID    PPID  C STIME TTY      STAT   TIME CMD
uikvel    339111     421  0 17:46 pts/4    S+     0:00 ./a.out
*/