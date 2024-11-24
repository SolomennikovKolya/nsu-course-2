
#include <iostream>
#include <mpi.h>
#include <random>
#include <time.h>
#include <unistd.h>
#include <vector>

int W = 10;                                         // ширина поля
int H = 10;                                         // высота поля
const int fillingDensity = 30;                      // плотность заполнения поля при случайном заполнении (от 0 до 100)
const char shiftX[8] = {1, 1, 0, -1, -1, -1, 0, 1}; // сдвиги соседнех клеткок по X
const char shiftY[8] = {0, -1, -1, -1, 0, 1, 1, 1}; // сдвиги соседнех клеткок по Y

// Класс исключений для отслеживания ошибок
class myException : public std::exception
{
private:
    const char *msg_;

public:
    explicit myException(const char *msg) : msg_(msg){};
    const char *what() const noexcept override { return msg_; }
};

// изменит размеры поля, если они переданы в аргументы коммандной строки
inline void initFieldSizes(int argc, char **argv)
{
    if (argc < 3)
        return;
    W = std::stoi(argv[1]);
    H = std::stoi(argv[2]);
}

// заполнение поля случайными клетками
inline void fillFieldRandom(std::vector<bool> &A)
{
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            A[y * W + x] = (rand() % 100 < fillingDensity);
}

// заполнение поля пустыми клетками, но с одним глайдером
inline void fillFieldWithGlider(std::vector<bool> &A)
{
    if (W < 3 || H < 3)
        throw myException("To make a field with a glider, you need to have a field of at least 3 by 3 cells");

    A[0 * W + 1] = true;
    A[1 * W + 2] = true;
    A[2 * W + 0] = true;
    A[2 * W + 1] = true;
    A[2 * W + 2] = true;
}

// инициализация и заполнение поля. Можно заполнить поле разными способами (0 - пустое поле, 1 - пустое с глайдером в углу, 2 - случайное)
inline void initData(std::vector<bool> *&A, const int kind)
{
    A = new std::vector<bool>(W * H, false);
    if (A == nullptr)
        throw myException("Malloc fail");

    if (kind == 1)
        fillFieldWithGlider(*A);
    else if (kind == 2)
        fillFieldRandom(*A);
}

// вывод поля на экран
inline void printField(std::vector<bool> &A)
{
    std::cout << "  ";
    for (int x = 0; x < W; ++x)
        std::cout << x % 10 << " ";
    std::cout << "\n";

    for (int y = 0; y < H; ++y)
    {
        std::cout << y % 10 << " ";
        for (int x = 0; x < W; ++x)
            std::cout << (A[y * W + x] ? '#' : '.') << " ";
        std::cout << "\n";
    }
}

// нормальный математический модуль (модуль отрицательного числа положительный)
inline int mod(int x, int m)
{
    return (x >= 0 ? x % m : x % m + m);
}

// просчитывание следующего поля
inline void generateNext(std::vector<bool> &A, std::vector<bool> &nextA)
{
    for (int y = 0; y < H; ++y)
    {
        for (int x = 0; x < W; ++x)
        {
            int sum = 0;
            for (int i = 0; i < 8; ++i)
                sum += int(A[mod(y + shiftY[i], H) * W + mod(x + shiftX[i], W)]);

            // условия выживания, рождения и умирания клетки соответственно
            if (sum == 2)
                nextA[y * W + x] = A[y * W + x];
            else if (sum == 3)
                nextA[y * W + x] = true;
            else
                nextA[y * W + x] = false;
        }
    }
}

// записывает вектор bool-ов в буфер в компактном виде
inline void writeBitVectorToBuffer(std::vector<bool> &A, unsigned char *const buf)
{
    int i = 0;
    for (; i < int(A.size()) / 8; ++i)
    {
        unsigned char c = 0;
        unsigned char mask = 1 << 7;
        c |= mask * A[i * 8];
        for (int j = 1; j < 8; ++j)
        {
            mask >>= 1;
            c |= mask * A[i * 8 + j];
        }
        buf[i] = c;
    }

    if (A.size() % 8 > 0)
    {
        unsigned char c = 0;
        unsigned char mask = 1 << 7;
        c |= mask * A[i * 8];
        for (int j = 1; j < int(A.size()) % 8; ++j)
        {
            mask >>= 1;
            c |= mask * A[i * 8 + j];
        }
        buf[i] = c;
    }
}

// добавляет поле в историю
inline void addFieldToHistory(std::vector<bool> &A, std::vector<unsigned char *> &history)
{
    const int capacity = (W * H + 7) / 8;
    unsigned char *buf = static_cast<unsigned char *>(malloc(sizeof(unsigned char) * capacity));
    if (buf == nullptr)
        throw myException("Malloc fail");

    writeBitVectorToBuffer(A, buf);
    history.push_back(buf);
}

// проверка на то, повторилось ли поле (возврящает -1 если поле ни разу не повторилось, иначе - номер поколения, с которым совпало текущее поле)
inline int checkFieldMatch(std::vector<unsigned char *> &history)
{
    unsigned char *A = history[history.size() - 1];
    unsigned char *B = nullptr;
    const int capacity = (W * H + 7) / 8;

    for (int gen = 0; gen < int(history.size()) - 1; ++gen)
    {
        B = history[gen];
        bool match = true;

        for (int i = 0; i < capacity; ++i)
        {
            if (A[i] != B[i])
            {
                match = false;
                break;
            }
        }
        if (match)
            return gen;
    }
    return -1;
}

// освобождение ресурсов истории полей
inline void freeHistory(std::vector<unsigned char *> &history)
{
    for (int i = 0; i < int(history.size()); ++i)
        free(history[i]);
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    const double startTime = MPI_Wtime();
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    srand(time(NULL));

    std::vector<bool> *A = nullptr;       // основное поле
    std::vector<bool> *nextA = nullptr;   // дополнительное поле
    std::vector<unsigned char *> history; // все предыдущие поля в сжатом виде
    int generation = 0;                   // номер поколения

    try
    {
        if (size != 1)
            throw myException("Sequential program must be run by a single process");

        initFieldSizes(argc, argv);
        initData(A, 1);
        initData(nextA, 0);
        addFieldToHistory(*A, history);

        while (true)
        {
            // printField(*A);

            generateNext(*A, *nextA);
            addFieldToHistory(*nextA, history);

            std::vector<bool> *tmp = A;
            A = nextA;
            nextA = tmp;
            generation++;

            const int matchedGenerstion = checkFieldMatch(history);
            if (matchedGenerstion != -1)
            {
                std::cout << "The fields matched on generations " << matchedGenerstion << " and " << generation << "\n";
                break;
            }
            // sleep(1);
        }

        const double endTime = MPI_Wtime();
        std::cout << "Total time: " << endTime - startTime << "\n";
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    delete A;
    delete nextA;
    freeHistory(history);
    MPI_Finalize();
    return 0;
}
