
#include <iostream>
#include <unistd.h>

void initBuf(int iter)
{
    std::cout << iter << " ";
    char a[4096];
    sleep(0.1);
    initBuf(iter + 1);
}

int main(int argc, char **argv)
{
    int pid = getpid();
    std::cout << "PID: " << pid << "\n";

    sleep(10);

    initBuf(0);
    // Нижняя граница стека постепенно сдвигается (стек растёт вниз) пока не достишнет лимита
    // В моём случае произошло 1860 итераций (это примерно 7Мб под буферы)
    return 0;
}
