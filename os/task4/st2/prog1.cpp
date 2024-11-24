
#include <iostream>
#include <unistd.h>

int main(int argc, char **argv)
{
    int pid = getpid();
    std::cout << "PID: " << pid << "\n"; // PID остаётся тем же, но адресное пространство полностью меняется

    sleep(1);

    execv(argv[0], argv);

    std::cout << "Hello world\n"; // никогда не выполнится (выполнится только если exec выдаст ошибку)
    return 1;
}