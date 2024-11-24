
#include <iostream>
#include <mpi.h>
#include <random>
#include <time.h>
#include <unistd.h>
#include <vector>

int W = 10;                                         // ширина поля
int H = 10;                                         // высота поля
const char shiftX[8] = {1, 1, 0, -1, -1, -1, 0, 1}; // сдвиги соседнех клеткок по X
const char shiftY[8] = {0, -1, -1, -1, 0, 1, 1, 1}; // сдвиги соседнех клеткок по Y
const int rootRank = 0;                             // номер корневого процесса
const int fillingDensity = 30;                      // плотность заполнения поля при случайном заполнении (от 0 до 100)
const int reallocDeltaSize = 10;                    // чтобы слишком часто не делать реаллокацию, буферы расширяются больше чем на самом деле надо

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

    A[0 * W + 1] = 1;
    A[1 * W + 2] = 1;
    A[2 * W + 0] = 1;
    A[2 * W + 1] = 1;
    A[2 * W + 2] = 1;
}

// инициализация и заполнение начального поля. Можно заполнить поле разными способами (0 - пустое поле, 1 - пустое с глайдером в углу, 2 - случайное)
inline void initStartField(char *&A, const int kind)
{
    A = static_cast<char *>(calloc(sizeof(char), W * H));
    if (A == nullptr)
        throw myException("Malloc fail");

    if (kind == 1)
        fillFieldWithGlider(A);
    else if (kind == 2)
        fillFieldRandom(A);
}

// вывод начального поля на экран
inline void printStartField(const char *const A)
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

// вывод текущего поля на экран по частям
inline void printCurFieldByParts(const char *const partA, const int partH, const int size, const int rank)
{
    for (int p = 0; p < size; ++p)
    {
        if (rank == p)
        {
            for (int y = 0; y < partH; ++y)
            {
                for (int x = 0; x < W; ++x)
                    std::cout << (partA[y * W + x] == 1 ? '#' : '.') << " ";
                std::cout << "\n";
            }
        }
        MPI_Barrier(MPI_COMM_WORLD);
    }
    // MPI_Barrier(MPI_COMM_WORLD);
    // if (rank == rootRank)
    //     std::cout << "\n";
}

// вывод текущего поля на экран по частям
inline void printCurFieldByGatherAllParts(char *const A, const char *const partA, const int *const sendcounts, const int *const displs, const int size, const int rank)
{
    MPI_Gatherv(partA, sendcounts[rank], MPI_CHAR, A, sendcounts, displs, MPI_CHAR, rootRank, MPI_COMM_WORLD);
    if (rank != rootRank)
        return;
    for (int y = 0; y < H; ++y)
    {
        for (int x = 0; x < W; ++x)
            std::cout << (A[y * W + x] == 1 ? '#' : '.') << " ";
        std::cout << "\n";
    }
    std::cout << "\n";
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
inline void initData(char *&partA, char *&nextPartA, char *&upLine, char *&downLine, const int partSize)
{
    partA = (char *)malloc(sizeof(char) * partSize);
    nextPartA = (char *)malloc(sizeof(char) * partSize);
    upLine = (char *)malloc(sizeof(char) * W);
    downLine = (char *)malloc(sizeof(char) * W);
    if (partA == nullptr || nextPartA == nullptr || upLine == nullptr || downLine == nullptr)
        throw myException("Malloc fail");
}

// инициализация или реаллокация flagList и allFlagsList
inline void initFlagsLists(unsigned char *&flagsList, unsigned char *&allFlagsList, const int size, const int generation)
{
    if (generation == 1)
    {
        flagsList = static_cast<unsigned char *>(calloc(sizeof(unsigned char), reallocDeltaSize));
        if (flagsList == nullptr)
            throw myException("Malloc fail");

        allFlagsList = static_cast<unsigned char *>(calloc(sizeof(unsigned char), size * reallocDeltaSize));
        if (allFlagsList == nullptr)
            throw myException("Malloc fail");
    }
    else if (generation % reallocDeltaSize == 0)
    {
        unsigned char *tmp = static_cast<unsigned char *>(realloc(flagsList, generation + reallocDeltaSize));
        if (tmp == nullptr)
            throw myException("Realloc fail");
        for (int i = generation; i < generation + reallocDeltaSize; ++i)
            tmp[i] = 0;
        flagsList = tmp;

        tmp = static_cast<unsigned char *>(realloc(allFlagsList, size * (generation + reallocDeltaSize)));
        if (tmp == nullptr)
            throw myException("Realloc fail");
        allFlagsList = tmp;
    }
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

// упаковывает массив char-ов, где каждый char это 0 или 1, в буфер, где каждый char - это 8 зашифрованных бит (A - массив для упаковки, size - количество элементов, который надо упаковать, buf - выходной буфер)
inline void packIntoBuffer(const char *const A, const int size, unsigned char *const buf)
{
    int i = 0;
    for (; i < size / 8; ++i)
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

    if (size % 8 > 0)
    {
        unsigned char c = 0;
        unsigned char mask = 1 << 7;
        c |= mask * A[i * 8];
        for (int j = 1; j < size % 8; ++j)
        {
            mask >>= 1;
            c |= mask * A[i * 8 + j];
        }
        buf[i] = c;
    }
}

// добавляет часть поля в историю (partA - часть поля, partH - высота части поля, history - вектор для хранения полей в компактном виде)
inline void addFieldToHistory(const char *const partA, const int partH, std::vector<unsigned char *> &history)
{
    const int capacity = (W * partH + 7) / 8;
    unsigned char *buf = static_cast<unsigned char *>(malloc(sizeof(unsigned char) * capacity));
    if (buf == nullptr)
        throw myException("Malloc fail");

    packIntoBuffer(partA, partH * W, buf);
    history.push_back(buf);
}

// устанавливает бит pos в значения val в буфере из char-ов
inline void setBit(unsigned char *const buf, const int pos, const bool val)
{
    unsigned char &byte = buf[pos / 8];
    unsigned char mask = (unsigned char)1 << (7 - pos % 8);
    if (val)
        byte |= mask;
    else
        byte -= byte & mask;
}

// прочитать бит pos в буфере из char-ов
inline bool getBit(const unsigned char *const buf, const int pos)
{
    unsigned char byte = buf[pos / 8];
    unsigned char mask = (unsigned char)1 << (7 - pos % 8);
    return ((byte & mask) > 0);
}

// выставление флагов (флаг = true, если последнее поле совпало с каким то из предыдущих, иначе false)
inline void checkFieldMatch(std::vector<unsigned char *> &history, unsigned char *flagsList, const int partH)
{
    unsigned char *curField = history[history.size() - 1];
    const int capacity = (W * partH + 7) / 8;

    for (int gen = 0; gen < int(history.size()) - 1; ++gen)
    {
        char match = true;
        for (int i = 0; i < capacity; ++i)
        {
            if (curField[i] != history[gen][i])
            {
                match = false;
                break;
            }
        }
        setBit(flagsList, gen, match);
    }
}

// пытается найти поколение, в котором на всех процессах совпали поля (возвращает номер найденного поколения или -1 если такого нет)
inline int checkAllFlags(const unsigned char *const allFlagsList, const int size, const int generation)
{
    const int capacity = (generation + 7) / 8;

    for (int i = 0; i < capacity; ++i)
    {
        unsigned char shouldCheck = allFlagsList[i];
        for (int p = 1; p < size; ++p)
            shouldCheck &= allFlagsList[capacity * p + i];
        if (shouldCheck == 0)
            continue;

        int ans = i * 8;
        unsigned mask = 1 << 7;
        for (int j = 0; j < 8; ++j)
        {
            if ((shouldCheck & mask) == 0)
            {
                mask >>= 1;
                ++ans;
            }
        }
        return ans;
    }
    return -1;
}

// освобождение памяти
inline void deleteData(char *&A, char *&partA, char *&nextPartA, int *&sendcounts, int *&displs, char *&upLine, char *&downLine, std::vector<unsigned char *> &history, unsigned char *&flagsList, unsigned char *&allFlagsList)
{
    free(A);
    free(partA);
    free(nextPartA);
    free(sendcounts);
    free(displs);
    free(upLine);
    free(downLine);
    for (int i = 0; i < int(history.size()); ++i)
        free(history[i]);
    free(flagsList);
    free(allFlagsList);
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
    char *partA = nullptr, *nextPartA = nullptr;  // часть поля и дополнительное поле для вычисления следующего поколения
    int *sendcounts = nullptr, *displs = nullptr; // для разделения поля A на части partA через scatterv
    char *upLine = nullptr, *downLine = nullptr;  // соседнии строчки сверху и снизу

    std::vector<unsigned char *> history;  // все предыдущие поля (нужно для определения, повторилось поле или нет) (!сжатый вид)
    int generation = 0;                    // номер поколения (или же номер последней записи поля в history)
    unsigned char *flagsList = nullptr;    // набор флагов (каждый флаг - совпало ли последняя часть поле с предыдущими частями поля) (!сжатый вид)
    unsigned char *allFlagsList = nullptr; // все флаги со всех процессов (!сжатый вид)

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
            initStartField(A, 1);
            // printStartField(A);
        }
        initSendcountsAndDispls(sendcounts, displs, size);
        initData(partA, nextPartA, upLine, downLine, sendcounts[rank]);

        MPI_Scatterv(A, sendcounts, displs, MPI_CHAR, partA, sendcounts[rank], MPI_CHAR, rootRank, MPI_COMM_WORLD); // раздача данных по процессам
        const int partH = sendcounts[rank] / W;                                                                     // высота части поля (высота partA и nextPartA)
        addFieldToHistory(partA, partH, history);                                                                   // добавление поля в историю

        while (true)
        {
            // printCurFieldByGatherAllParts(A, partA, sendcounts, displs, size, rank);

            // инициализация передачи верхней и нижней строчки соседним процессам
            MPI_Request sendRequests[2];
            MPI_Isend(partA, W, MPI_CHAR, mod(rank - 1, size), 1, MPI_COMM_WORLD, &sendRequests[0]);                          // отправка первой строки предыдущему процессу
            MPI_Isend(partA + (sendcounts[rank] - W), W, MPI_CHAR, mod(rank + 1, size), 2, MPI_COMM_WORLD, &sendRequests[1]); // отправка последней строки следующему процессу
            MPI_Request recvRequests[2];
            MPI_Irecv(downLine, W, MPI_CHAR, mod(rank + 1, size), 1, MPI_COMM_WORLD, &recvRequests[0]); // получение нижней строки от следующего процесса
            MPI_Irecv(upLine, W, MPI_CHAR, mod(rank - 1, size), 2, MPI_COMM_WORLD, &recvRequests[1]);   // получение верхней строки от предыдущего процесса

            // сравнение текущей части поля с предыдущими частями (сравнение последней записи в history со всеми остальными записями)
            // на нулевом поколении нечего сравнивать (в history должно быть хотя бы две записи)
            MPI_Request allgatherRequest;
            if (generation > 0)
            {
                initFlagsLists(flagsList, allFlagsList, size, generation);
                // вычисление вектора флагов
                checkFieldMatch(history, flagsList, partH);
                // инициализация сбора всех флагов в allFlagsList
                MPI_Iallgather(flagsList, (generation + 7) / 8, MPI_UNSIGNED_CHAR, allFlagsList, (generation + 7) / 8, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD, &allgatherRequest);
            }

            // вычисление внутренней части (всё кроме первой и последней строки)
            calculateInnerField(partA, nextPartA, partH);

            // вычислени крайних строк
            int firstRequest;
            MPI_Waitany(2, recvRequests, &firstRequest, MPI_STATUS_IGNORE);
            if (firstRequest == 1)
            {
                calculateUpLine(partA, upLine, nextPartA);
                MPI_Wait(&recvRequests[0], MPI_STATUS_IGNORE);
                calculateDownLine(partA, downLine, nextPartA, partH);
            }
            else
            {
                calculateDownLine(partA, downLine, nextPartA, partH);
                MPI_Wait(&recvRequests[1], MPI_STATUS_IGNORE);
                calculateUpLine(partA, upLine, nextPartA);
            }

            // добавление поля в историю
            addFieldToHistory(nextPartA, partH, history);

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
            char *tmp = partA;
            partA = nextPartA;
            nextPartA = tmp;
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

    deleteData(A, partA, nextPartA, sendcounts, displs, upLine, downLine, history, flagsList, allFlagsList);
    MPI_Finalize();
    return 0;
}

// чтобы принтовать расшифрованный allFlagsList
// const int capacity = (generation + 7) / 8;
// if (rank == rootRank)
// {
//     for (int p = 0; p < size; ++p)
//     {
//         for (int i = 0; i < capacity; ++i)
//         {
//             unsigned char mask = (unsigned char)1 << 7;
//             std::cout << ((allFlagsList[capacity * p + i] & mask) == 0 ? 0 : 1);
//             for (int j = 0; j < 7; ++j)
//             {
//                 mask >>= 1;
//                 std::cout << ((allFlagsList[capacity * p + i] & mask) == 0 ? 0 : 1);
//             }
//         }
//         std::cout << "\n";
//     }
//     std::cout << "\n";
// }

// чтобы принтовать расшифрованную исторую
// const int capacity = (W * partH + 7) / 8;
// if (rank == rootRank)
// {
//     for (int h = 0; h < history.size(); ++h)
//     {
//         for (int i = 0; i < capacity; ++i)
//         {
//             unsigned char mask = (unsigned char)1 << 7;
//             std::cout << ((history[h][i] & mask) == 0 ? 0 : 1);
//             for (int j = 0; j < 7; ++j)
//             {
//                 mask >>= 1;
//                 std::cout << ((history[h][i] & mask) == 0 ? 0 : 1);
//             }
//         }
//         std::cout << "\n";
//     }
// }