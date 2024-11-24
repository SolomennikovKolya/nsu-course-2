
#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

template <typename T>
void printBinary(T num)
{
    for (int i = sizeof(num) * 8 - 1; i >= 0; --i)
    {
        std::cout << ((num >> i) & 1);
    }
}

class myHeapClass
{
private:
    size_t size = 1024 * 1024; // максимальный размер кучи (1Мб)
    int fd;                    // файловый дескриптор
    void *heap;                // начало региона памяти
    bool valid = false;        // становится true только после успешного создания кучи

public:
    myHeapClass()
    {
        fd = open("./heap.txt", O_RDWR | O_TRUNC | O_CREAT, 644);
        if (fd == -1)
        {
            std::cerr << "Failed to open file\n";
            return;
        }
        // Устанавливаем размер файла
        if (ftruncate(fd, size) == -1)
        {
            perror("ftruncate");
            close(fd);
            return;
        }

        // Создание региона памяти
        // heap = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        heap = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (heap == MAP_FAILED)
        {
            perror("mmap");
            close(fd);
            return;
        }

        *static_cast<size_t *>(heap) = 0;
        valid = true;
    }

    ~myHeapClass()
    {
        if (munmap(heap, size) == -1)
            perror("munmap");
        close(fd);
    }

    void printHeapFileStruct()
    {
        if (!valid)
        {
            std::cout << "Heap is not valid\n";
            return;
        }

        size_t shift = 0;
        size_t sumBytes = 0;
        while (true)
        {
            const size_t curBlockSize = *static_cast<size_t *>(heap + shift);
            const char curBlockFlag = *static_cast<char *>(heap + shift + 8);
            if (curBlockSize == 0)
                break;

            std::cout << "[size = " << curBlockSize << ", flag = " << int(curBlockFlag) << "] bytes: 8 + 1 + " << curBlockSize << " = " << 8 + 1 + curBlockSize << "\n";
            sumBytes += 8 + 1 + curBlockSize;
            shift += curBlockSize + 8 + 1;
        }
        std::cout << "In total " << sumBytes << " bytes used in heap\n";
    }

    friend void *myMalloc(size_t newBlockSize);
    friend void myFree(void *ptr);
};

// Глобальная статическая куча
myHeapClass myHeap;

void *myMalloc(size_t newBlockSize)
{
    if (!myHeap.valid || newBlockSize == 0)
        return NULL;

    size_t shift = 0;

    while (true)
    {
        if (shift + 8 + 1 + newBlockSize > myHeap.size)
            return NULL; // нехватка памяти в куче

        const size_t curBlockSize = *static_cast<size_t *>(myHeap.heap + shift);
        const char curBlockFlag = *static_cast<char *>(myHeap.heap + shift + 8);

        if (curBlockSize == 0) // достигнут конец кучи (можно создать новый блок)
        {
            *static_cast<size_t *>(myHeap.heap + shift) = newBlockSize;
            *static_cast<char *>(myHeap.heap + shift + 8) = 1;
            return static_cast<void *>(myHeap.heap + shift + 8 + 1);
        }
        else if (curBlockFlag == 0 && curBlockSize >= newBlockSize) // незанятый блок посреди кучи
        {
            *static_cast<char *>(myHeap.heap + shift + 8) = 1;
            return static_cast<void *>(myHeap.heap + shift + 8 + 1);
        }
        else
        {
            shift += curBlockSize + 8 + 1;
        }
    }
    return NULL;
}

void myFree(void *ptr)
{
    if (ptr < myHeap.heap + 8 + 1 || ptr > myHeap.heap + myHeap.size - 1)
    {
        std::cerr << "Invalid pointer\n";
        return;
    }
    const char lastFlag = *static_cast<char *>(ptr - 1);
    if (lastFlag == 0)
        return; // блок и так свободен
    else if (lastFlag == 1)
        *static_cast<char *>(ptr - 1) = 0;
    else
        std::cerr << "Invalid pointer\n";
}

// напечатает первые len байтов файла для кучи
void printHeapFile(size_t len)
{
    std::ifstream file("./heap.txt", std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Failed to open file\n";
        return;
    }

    char byte;
    const int bytesInLine = 16;
    for (int i = 0; i < (len + bytesInLine - 1) / bytesInLine; ++i)
    {
        for (int j = 0; j < bytesInLine && i * bytesInLine + j < len; ++j)
            std::cout << i * bytesInLine + j + 1 << "\t";
        std::cout << "\n";

        for (int j = 0; j < bytesInLine && i * bytesInLine + j < len; ++j)
        {
            if (!file.get(byte))
            {
                std::cerr << "Failed to print heap file\n";
                return;
            }
            printBinary<char>(byte);
        }
        std::cout << "\n";
    }
    file.close();
}

int main()
{
    char *a = (char *)myMalloc(sizeof(char) * 10);
    for (int i = 0; i < 10; ++i)
        a[i] = i;

    char *b = (char *)myMalloc(sizeof(char) * 10);
    // myFree(b);

    long long *c = (long long *)myMalloc(sizeof(long long) * 10);
    for (int i = 0; i < 5; i++)
        c[i] = 63;

    // int *d = (int *)myMalloc(sizeof(int) * 2);

    printHeapFile(127);
    myHeap.printHeapFileStruct();

    myFree(a);
    myFree(b);
    // myFree(d);

    return 0;
}