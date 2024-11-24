
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

// заполнение поля пустыми клетками
inline void fillFieldEmpty(char *const A)
{
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            A[y * W + x] = 0;
}

// заполнение поля случайными клетками
inline void fillFieldRandom(char *const A)
{
    for (int y = 0; y < H; ++y)
        for (int x = 0; x < W; ++x)
            A[y * W + x] = (rand() % 100 < fillingDensity ? 1 : 0);
}

// заполнение поля пустыми клетками, но с одним глайдером
inline void fillFieldWithGlider(char *const A)
{
    if (W < 3 || H < 3)
        throw myException("To make a field with a glider, you need to have a field of at least 3 by 3 cells");

    fillFieldEmpty(A);
    A[0 * W + 1] = 1;
    A[1 * W + 2] = 1;
    A[2 * W + 0] = 1;
    A[2 * W + 1] = 1;
    A[2 * W + 2] = 1;
}

// вывод поля на экран
inline void printField(const char *const A)
{
    std::cout << "  ";
    for (int x = 0; x < W; ++x)
        std::cout << x % 10 << " ";
    std::cout << "\n";

    for (int y = 0; y < H; ++y)
    {
        std::cout << y % 10 << " ";
        for (int x = 0; x < W; ++x)
            std::cout << (A[y * W + x] == 1 ? '#' : '.') << " ";
        std::cout << "\n";
    }
}

// инициализация и заполнения поля
inline void initData(char *&A)
{
    A = static_cast<char *>(malloc(sizeof(char) * W * H));
    if (A == nullptr)
        throw myException("Malloc fail");

    // fillFieldRandom(A);
    fillFieldWithGlider(A);
}

// освобождение ресурсов истории полей
inline void freeHistory(std::vector<char *> &history, const int generation)
{
    for (int i = 0; i < generation; ++i)
        free(history[i]);
}

// нормальный математический модуль (модуль отрицательного числа положительный)
inline int mod(int x, int m)
{
    return (x >= 0 ? x % m : x % m + m);
}

// просчитывание следующего поля
inline void generateNext(const char *const A, char *const nextA)
{
    for (int y = 0; y < H; ++y)
    {
        for (int x = 0; x < W; ++x)
        {
            int sum = 0;
            for (int i = 0; i < 8; ++i)
                sum += A[mod(y + shiftY[i], H) * W + mod(x + shiftX[i], W)];

            // условия выживания, рождения и умирания клетки соответственно
            if (sum == 2)
                nextA[y * W + x] = A[y * W + x];
            else if (sum == 3)
                nextA[y * W + x] = 1;
            else
                nextA[y * W + x] = 0;
        }
    }
}

// проверка на то, повторилось ли поле (возврящает -1 если поле ни разу не повторилось, иначе - номер поколения, с которым совпало текущее поле)
inline int checkFieldMatch(const char *const A, std::vector<char *> &history)
{
    for (int gen = 0; gen < history.size(); ++gen)
    {
        const char *const B = history[gen];
        bool match = true;
        for (int y = 0; y < H; ++y)
        {
            for (int x = 0; x < W; ++x)
            {
                if (A[y * W + x] != B[y * W + x])
                {
                    match = false;
                    break;
                }
            }
            if (!match)
                break;
        }
        if (match)
            return gen;
    }
    return -1;
}

int main(int argc, char **argv)
{
    MPI_Init(&argc, &argv);
    const double startTime = MPI_Wtime();
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    srand(time(NULL));

    char *A = nullptr, *nextA = nullptr; // основное поле и дополнительное
    std::vector<char *> history;         // все предыдущие поля
    int generation = 0;                  // номер поколения

    try
    {
        if (size != 1)
            throw myException("Sequential program must be run by a single process");
        initFieldSizes(argc, argv);
        initData(A);

        while (true)
        {
            nextA = static_cast<char *>(malloc(sizeof(char) * W * H));
            if (nextA == nullptr)
                throw myException("Malloc fail");

            // printField(A);
            generateNext(A, nextA);
            history.push_back(A);
            A = nextA;
            nextA = nullptr;
            generation++;

            const int matchedGenerstion = checkFieldMatch(A, history);
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

    free(A);
    freeHistory(history, generation);
    MPI_Finalize();
    return 0;
}
