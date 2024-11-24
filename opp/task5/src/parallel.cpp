
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
const int rootRank = 0;

// Класс исключений для отслеживания ошибок
class myException : public std::exception
{
private:
    const char *msg_;
    const bool outputByOnlyRoot_; // флаг для того, нужно ли обрабатывать исключения принятные не от корневого процесса

public:
    explicit myException(const char *msg) : msg_(msg), outputByOnlyRoot_(false){};
    myException(const char *msg, bool outputByOnlyRoot) : msg_(msg), outputByOnlyRoot_(outputByOnlyRoot){};
    const char *what() const noexcept override { return msg_; }
    bool isOutputByOnlyRoot() const noexcept { return outputByOnlyRoot_; }
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

// инициализация и заполнения начального поля
inline void initStartField(char *&A)
{
    A = static_cast<char *>(malloc(sizeof(char) * W * H));
    if (A == nullptr)
        throw myException("Malloc fail");

    // fillFieldRandom(A);
    fillFieldWithGlider(A);
}

// Инициализация и заполнение массивов для размеров частей и их начальных позиций
inline void initSendcountsAndDispls(int *&sendcounts, int *&displs, const int size)
{
    sendcounts = (int *)malloc(sizeof(int) * size);
    displs = (int *)malloc(sizeof(int) * size);
    if (sendcounts == nullptr || displs == nullptr)
        throw myException("Malloc fail");

    const int smallParts = size - H % size; // количество маленьких кусков
    const int smallPartSize = H / size;     // высота маленьких кусков
    const int bigParts = H % size;          // количество больших кусков
    const int bigPartSize = H / size + 1;   // высота больших кусков
    int pos = 0;

    for (int p = 0; p < smallParts; ++p)
    {
        sendcounts[p] = smallPartSize * W;
        displs[p] = pos;
        pos += sendcounts[p];
    }
    for (int p = smallParts; p < size; ++p)
    {
        sendcounts[p] = bigPartSize * W;
        displs[p] = pos;
        pos += sendcounts[p];
    }
}

// инициализация буферов для кусков поля и строк соседних частей
inline void initData(char *&A, char *&partA, char *&nextPartA, char *&upLine, char *&downLine, const int partSize)
{
    partA = (char *)malloc(sizeof(char) * partSize);
    nextPartA = (char *)malloc(sizeof(char) * partSize);
    upLine = (char *)malloc(sizeof(char) * W);
    downLine = (char *)malloc(sizeof(char) * W);
    if (partA == nullptr || nextPartA == nullptr || upLine == nullptr || downLine == nullptr)
        throw myException("Malloc fail");
}

// инициализация или реаллокация allFlagsList
inline void initAllFlagsList(bool *&allFlagsList, const int size, const int generation)
{
    if (generation == 1)
    {
        allFlagsList = static_cast<bool *>(malloc(sizeof(bool) * size));
        if (allFlagsList == nullptr)
            throw myException("Malloc fail");
    }
    else
    {
        bool *tmp = static_cast<bool *>(realloc(allFlagsList, size * generation));
        if (tmp == nullptr)
            throw myException("Malloc fail");
        allFlagsList = tmp;
    }
}

// освобождение памяти
inline void deleteData(char *&A, char *&partA, char *&nextPartA, int *&sendcounts, int *&displs, char *&upLine, char *&downLine, std::vector<char *> &history, bool *&allFlagsList)
{
    free(A);
    free(partA);
    free(nextPartA);
    free(sendcounts);
    free(displs);
    free(upLine);
    free(downLine);
    for (int i = 0; i < history.size(); ++i)
        free(history[i]);
    free(allFlagsList);
}

// нормальный математический модуль (модуль отрицательного числа положительный)
inline int mod(int x, int m)
{
    return (x >= 0 ? x % m : x % m + m);
}

// просчитывание следующего поля (внутренный части поля)
inline void calculateInnerField(const char *const partA, char *const nextPartA, const int partH)
{
    for (int y = 1; y < partH - 1; ++y)
    {
        for (int x = 0; x < W; ++x)
        {
            int sum = 0;
            for (int i = 0; i < 8; ++i)
                sum += partA[(y + shiftY[i]) * W + mod(x + shiftX[i], W)];

            // условия выживания, рождения и умирания клетки соответственно
            if (sum == 2)
                nextPartA[y * W + x] = partA[y * W + x];
            else if (sum == 3)
                nextPartA[y * W + x] = 1;
            else
                nextPartA[y * W + x] = 0;
        }
    }
}

// просчитывание следующего поля (верхняя линия)
inline void calculateUpLine(const char *const partA, const char *const upLine, char *const nextPartA)
{
    for (int x = 0; x < W; ++x)
    {
        int sum = 0;
        for (int i = 0; i < 8; ++i)
        {
            if (shiftY[i] == -1)
                sum += upLine[mod(x + shiftX[i], W)];
            else
                sum += partA[shiftY[i] * W + mod(x + shiftX[i], W)];
        }

        // условия выживания, рождения и умирания клетки соответственно
        if (sum == 2)
            nextPartA[x] = partA[x];
        else if (sum == 3)
            nextPartA[x] = 1;
        else
            nextPartA[x] = 0;
    }
}

// просчитывание следующего поля (нижняя линия)
inline void calculateDownLine(const char *const partA, const char *const downLine, char *const nextPartA, const int partH)
{
    for (int x = 0; x < W; ++x)
    {
        int sum = 0;
        for (int i = 0; i < 8; ++i)
        {
            if (shiftY[i] == 1)
                sum += downLine[mod(x + shiftX[i], W)];
            else
                sum += partA[(partH - 1 + shiftY[i]) * W + mod(x + shiftX[i], W)];
        }

        // условия выживания, рождения и умирания клетки соответственно
        if (sum == 2)
            nextPartA[(partH - 1) * W + x] = partA[(partH - 1) * W + x];
        else if (sum == 3)
            nextPartA[(partH - 1) * W + x] = 1;
        else
            nextPartA[(partH - 1) * W + x] = 0;
    }
}

// проверка на то, повторилось ли поле (выставляет соответствующие флаги)
inline void checkFieldMatch(const char *const A, std::vector<char *> &history, std::vector<char> &flagsList, const int partH)
{
    flagsList.push_back(0); // просто увеличение размера вектора на 1

    for (int gen = 0; gen < history.size(); ++gen)
    {
        char match = 1;
        for (int y = 0; y < partH; ++y)
        {
            for (int x = 0; x < W; ++x)
            {
                if (A[y * W + x] != history[gen][y * W + x])
                {
                    match = 0;
                    break;
                }
            }
            if (!match)
                break;
        }
        flagsList[gen] = match;
    }
}

// пытается найти поколение, в котором на всех процессах совпали поля (возвращает номер найденного поколения или -1 если такого нет)
inline int checkAllFlags(const bool *const allFlagsList, const int size, const int generation)
{
    for (int gen = 0; gen < generation; ++gen)
    {
        bool flag = true;
        for (int p = 0; p < size; ++p)
        {
            if (allFlagsList[p * generation + gen] != true)
            {
                flag = false;
                break;
            }
        }
        if (flag)
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

    char *A = nullptr;                            // целое поле (генерированнуется корневым процессом и раздаётся частями partA остальным процессам)
    char *partA = nullptr, *nextPartA = nullptr;  // часть поля и дополнительный буфер для хранения следующего поколения поля
    int *sendcounts = nullptr, *displs = nullptr; // для разделения поля A на части partA через scatterv
    char *upLine = nullptr, *downLine = nullptr;  // строки из частей поля соседних процессов, смежные с краями поля данного процесса
    std::vector<char *> history;                  // все предыдущие поля (нужно для определения, повторилось поле или нет)
    int generation = 0;                           // номер поколения
    std::vector<char> flagsList;                  // набор флагов (совпадает ли текущая часть поле с предыдущими частями поля)
    bool *allFlagsList = nullptr;                 // все флаги со всех процессов

    try
    {
        initFieldSizes(argc, argv);
        if (size < 2)
            throw myException("The parallel program must run on at least two processes", true);
        if (size * 2 > H)
            throw myException("The field sizes are too small for so many processes", true);

        // инициализация данных
        if (rank == rootRank)
        {
            initStartField(A);
            // printField(A);
        }
        initSendcountsAndDispls(sendcounts, displs, size);
        initData(A, partA, nextPartA, upLine, downLine, sendcounts[rank]);

        // раздача данных по процессам
        MPI_Scatterv(A, sendcounts, displs, MPI_CHAR, partA, sendcounts[rank], MPI_CHAR, rootRank, MPI_COMM_WORLD);

        while (true)
        {
            // инициализация передачи верхней и нижней строчки соседним процессам
            MPI_Request sendRequests[2];
            MPI_Isend(partA, W, MPI_CHAR, mod(rank - 1, size), 1, MPI_COMM_WORLD, &sendRequests[0]);                          // отправка первой строки предыдущему процессу
            MPI_Isend(partA + (sendcounts[rank] - W), W, MPI_CHAR, mod(rank + 1, size), 2, MPI_COMM_WORLD, &sendRequests[1]); // отправка последней строки следующему процессу
            MPI_Request recvRequests[2];
            MPI_Irecv(downLine, W, MPI_CHAR, mod(rank + 1, size), 1, MPI_COMM_WORLD, &recvRequests[0]); // получение нижней строки от следующего процесса
            MPI_Irecv(upLine, W, MPI_CHAR, mod(rank - 1, size), 2, MPI_COMM_WORLD, &recvRequests[1]);   // получение верхней строки от предыдущего процесса

            // сравнение текущего поля с предыдущими (но на нулевом поколении пока нечего сравнивать, так как история пуста)
            MPI_Request allgatherRequest;
            if (generation > 0)
            {
                // вычисление вектора флагов
                checkFieldMatch(partA, history, flagsList, sendcounts[rank] / W);
                // инициализация сбора всех флагов в allFlagsList
                initAllFlagsList(allFlagsList, size, generation);
                MPI_Iallgather(flagsList.data(), generation, MPI_CHAR, allFlagsList, generation, MPI_CHAR, MPI_COMM_WORLD, &allgatherRequest);
            }

            // инициализация следующего поля
            nextPartA = static_cast<char *>(malloc(sizeof(char) * sendcounts[rank]));
            if (nextPartA == nullptr)
                throw myException("Malloc fail");

            // вычисление внутренней части (всё кроме первой и последней строки)
            calculateInnerField(partA, nextPartA, sendcounts[rank] / W);

            // вычислени крайних строк
            int firstRequest;
            MPI_Waitany(2, recvRequests, &firstRequest, MPI_STATUS_IGNORE);
            if (firstRequest == 1)
            {
                calculateUpLine(partA, upLine, nextPartA);
                MPI_Wait(&recvRequests[0], MPI_STATUS_IGNORE);
                calculateDownLine(partA, downLine, nextPartA, sendcounts[rank] / W);
            }
            else
            {
                calculateDownLine(partA, downLine, nextPartA, sendcounts[rank] / W);
                MPI_Wait(&recvRequests[1], MPI_STATUS_IGNORE);
                calculateUpLine(partA, upLine, nextPartA);
            }

            // закрытие реквестов
            MPI_Status ignores[2];
            MPI_Waitall(2, sendRequests, ignores);

            // проверка флагов
            if (generation > 0)
            {
                MPI_Wait(&allgatherRequest, MPI_STATUS_IGNORE);
                const int matchedGenerstion = checkAllFlags(allFlagsList, size, generation);
                if (matchedGenerstion != -1)
                {
                    if (rank == rootRank)
                        std::cout << "The fields matched on generations " << matchedGenerstion << " and " << generation << "\n";
                    break;
                }
            }

            // переход на новое поколение (перекидывание указателей)
            history.push_back(partA);
            partA = nextPartA;
            nextPartA = nullptr;
            generation++;
        }

        const double endTime = MPI_Wtime();
        if (rank == rootRank)
            std::cout << "Total time: " << endTime - startTime << "\n";
    }
    catch (const myException &e)
    {
        if (!e.isOutputByOnlyRoot() || rank == rootRank)
            std::cerr << e.what() << '\n';
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    deleteData(A, partA, nextPartA, sendcounts, displs, upLine, downLine, history, allFlagsList);
    MPI_Finalize();
    return 0;
}

// for (int p = 0; p < size; ++p)
// {
//     if (rank == p)
//     {
//         for (int y = 0; y < sendcounts[rank] / W; ++y)
//         {
//             for (int x = 0; x < W; ++x)
//                 std::cout << (partA[y * W + x] == 1 ? '#' : '.') << " ";
//             std::cout << "\n";
//         }
//     }
//     MPI_Barrier(MPI_COMM_WORLD);
// }
// MPI_Barrier(MPI_COMM_WORLD);
// if (rank == rootRank)
//     std::cout << "\n";

// if (rank == 1)
// {
//     for (int p = 0; p < size; ++p)
//     {
//         for (int gen = 0; gen < generation; ++gen)
//             std::cout << (allFlagsList[p * generation + gen] ? "1" : "0") << " ";
//         std::cout << "\n";
//     }
//     std::cout << "\n";
// }
// MPI_Barrier(MPI_COMM_WORLD);
