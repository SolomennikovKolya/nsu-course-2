
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <unistd.h>

int globalInitializedVar = 4; // Глобальная инициализированная переменная
int globalUninitializedVar;   // Глобальная неициализированная переменная
const int globalConstVar = 6; // Глобальная константа

void foo()
{
    int localVar = 1;              // Локальная переменная
    static int localStaticVar = 2; // Локальная статическая переменная
    const int localConstVar = 3;   // Локальная константная переменная

    // Вывод адресов переменных
    printf("Address of local var:                %p\n", &localVar);               // stack
    printf("Address of local static var:         %p\n", &localStaticVar);         // data (rw-)
    printf("Address of local const var:          %p\n", &localConstVar);          // stack
    printf("Address of global initialized var:   %p\n", &globalInitializedVar);   // data (rw-)
    printf("Address of global uninitialized var: %p\n", &globalUninitializedVar); // data (rw-) (bss)
    printf("Address of global const var:         %p\n", &globalConstVar);         // data (r--)
}

int *localVarInit()
{
    int x = 10;
    return &x;
}

void testBuffer()
{
    char *buf = (char *)malloc(sizeof(char) * 100);
    strcpy(buf, "hello world");
    std::cout << "1. " << buf << "\n";

    free(buf);
    std::cout << "2. " << buf << "\n"; // выведет мусор

    char *buf2 = (char *)malloc(sizeof(char) * 100);
    strcpy(buf2, "hello world");
    std::cout << "3. " << buf << "\n";

    buf2 += 50;
    free(buf2); // free invalid pointer
    std::cout << "4. " << buf << "\n";
}

void setEnvVar()
{
    std::cout << "MY_VAR: " << getenv("MY_VAR") << "\n";

    setenv("MY_VAR", "qwe", 1);

    std::cout << "MY_VAR: " << getenv("MY_VAR") << "\n";
}

int main()
{
    foo();
    std::cout << "PID: " << getpid() << "\n";
    sleep(1000);

    // int *localVarAddress = localVarInit();
    // std::cout << localVarAddress << "\n";

    // testBuffer();

    setEnvVar();

    return 0;
}
