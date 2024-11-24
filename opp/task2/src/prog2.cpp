
#include "config.h"

#include <cmath>
#include <exception>
#include <iostream>
#include <mpi.h>

// Класс исключений для отслеживания ошибок
class myException : public std::exception
{
private:
    const char *msg_;

public:
    explicit myException(const char *msg) : msg_(msg){};
    const char *what() const noexcept override { return msg_; }
};

// Инициализация и заполнение массивов для размеров частей и их начальных позиций
void initSendcountsAndDispls(int *&sendcountsA, int *&sendcountsB, int *&displsA, int *&displsB, const int size)
{
    const int bigParts = N % size;
    const int smallPartSize = N / size;
    int pos = 0;

    sendcountsA = (int *)malloc(sizeof(int) * size);
    sendcountsB = (int *)malloc(sizeof(int) * size);
    displsA = (int *)malloc(sizeof(int) * size);
    displsB = (int *)malloc(sizeof(int) * size);
    if (sendcountsA == nullptr || sendcountsB == nullptr || displsA == nullptr || displsB == nullptr)
        throw myException("Malloc fail");

    for (int p = 0; p < bigParts; ++p)
    {
        sendcountsA[p] = (smallPartSize + 1) * N;
        displsA[p] = pos;
        pos += (smallPartSize + 1) * N;
    }
    for (int p = bigParts; p < size; ++p)
    {
        sendcountsA[p] = smallPartSize * N;
        displsA[p] = pos;
        pos += smallPartSize * N;
    }

    pos = 0;
    for (int p = 0; p < bigParts; ++p)
    {
        sendcountsB[p] = smallPartSize + 1;
        displsB[p] = pos;
        pos += smallPartSize + 1;
    }
    for (int p = bigParts; p < size; ++p)
    {
        sendcountsB[p] = smallPartSize;
        displsB[p] = pos;
        pos += smallPartSize;
    }
}

// Инициализация буферов для корневого процесса
void initRootData(double *&A, double *&b, double *&partA, double *&partB, double *&x, double *&res, double *&partRes, const int *const &sendcountsA, const int *const &sendcountsB)
{
    A = (double *)malloc(sizeof(double) * (N * N));
    if (A == nullptr)
        throw myException("Malloc fail");
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            if (i >= N)
                A[i * N + j] = 0.0;
            else if (i == j)
                A[i * N + j] = 2.0;
            else
                A[i * N + j] = 1.0;
        }
    }

    b = (double *)malloc(sizeof(double) * N);
    if (b == nullptr)
        throw myException("Malloc fail");
    for (int i = 0; i < N; ++i)
        b[i] = N + 1.0;

    partA = (double *)malloc(sizeof(double) * sendcountsA[0]);
    partB = (double *)malloc(sizeof(double) * sendcountsB[0]);
    if (partA == nullptr || partB == nullptr)
        throw myException("Malloc fail");

    x = (double *)malloc(sizeof(double) * N);
    if (x == nullptr)
        throw myException("Malloc fail");
    for (int i = 0; i < N; ++i)
        x[i] = sin(i);

    res = (double *)malloc(sizeof(double) * N);
    partRes = (double *)malloc(sizeof(double) * sendcountsB[0]);
    if (res == nullptr || partRes == nullptr)
        throw myException("Malloc fail");
}

// Инициализация буферов для не корневых процессов
void initSideData(double *&partA, double *&partB, double *&x, double *&partRes, const int *const &sendcountsA, const int *const &sendcountsB, const int rank)
{
    partA = (double *)malloc(sizeof(double) * sendcountsA[rank]);
    partB = (double *)malloc(sizeof(double) * sendcountsB[rank]);
    x = (double *)malloc(sizeof(double) * N);
    partRes = (double *)malloc(sizeof(double) * sendcountsB[rank]);
    if (partA == nullptr || partB == nullptr || x == nullptr || partRes == nullptr)
        throw myException("Malloc fail");
}

// Освобождение ресурсов
void deleteData(double *&A, double *&b, double *&partA, double *&partB, double *&x, double *&res, double *&partRes, int *&sendcountsA, int *&sendcountsB, int *&displsA, int *&displsB)
{
    free(A);
    free(b);
    free(partA);
    free(partB);
    free(x);
    free(res);
    free(partRes);
    free(sendcountsA);
    free(sendcountsB);
    free(displsA);
    free(displsB);
}

// partRes = partA * x
void mul(const double *const &partA, const double *const &x, double *const &partRes, const int partSize)
{
    for (int i = 0; i < partSize; ++i)
    {
        partRes[i] = 0;
        const int iN = i * N;
        for (int j = 0; j < N; ++j)
            partRes[i] += partA[iN + j] * x[j];
    }
}

// partRes = partRes * TAU
void mulTAU(double *const &partRes, const int partSize)
{
    for (int i = 0; i < partSize; ++i)
        partRes[i] *= TAU;
}

// partRes = partRes - partB
void sub(double *const &partRes, const double *const &partB, const int partSize)
{
    for (int i = 0; i < partSize; ++i)
        partRes[i] -= partB[i];
}

// partRes = partX - partRes
void subPartX(double *const &partRes, const double *const &x, const int partSize, const int startPos)
{
    for (int i = 0; i < partSize; ++i)
        partRes[i] = x[startPos + i] - partRes[i];
}

// Квадрат модуля вектора длины N
double mod2(const double *const &x)
{
    double ans = 0;
    for (int i = 0; i < N; ++i)
        ans += x[i] * x[i];
    return ans;
}

// Клпиркет первые N элементов из x в res
void copy(const double *const &x, double *const &res)
{
    for (int i = 0; i < N; ++i)
        res[i] = x[i];
}

// Модуль для double
double abs(const double a)
{
    return a < 0 ? -a : a;
}

// Проверка ответа на корректность
void testAns(const double *const &res)
{
    for (int i = 0; i < N; ++i)
    {
        // std::cout << res[i]  << " ";
        // std::cout << abs(res[i] - 1.0) << " ";
        if (abs(res[i] - 1.0) > CALCULATION_ERROR)
        {
            fprintf(stderr, "res[%d] = %lf, but expected 1.0\n", i, res[i]);
            throw myException("Wrong result");
        }
    }
}

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    double *A = nullptr, *b = nullptr, *x = nullptr, *res = nullptr;
    double *partA = nullptr, *partB = nullptr, *partRes = nullptr;
    int *sendcountsA = nullptr, *sendcountsB = nullptr;
    int *displsA = nullptr, *displsB = nullptr;
    const double startTime = MPI_Wtime();

    try
    {
        // Инициализация начальных данных
        initSendcountsAndDispls(sendcountsA, sendcountsB, displsA, displsB, size);
        if (rank == 0)
            initRootData(A, b, partA, partB, x, res, partRes, sendcountsA, sendcountsB);
        else
            initSideData(partA, partB, x, partRes, sendcountsA, sendcountsB, rank);

        // Раздача данных не корневым процессам
        MPI_Scatterv(A, sendcountsA, displsA, MPI_DOUBLE, partA, sendcountsA[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Scatterv(b, sendcountsB, displsB, MPI_DOUBLE, partB, sendcountsB[rank], MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        int iters = 0;        // Количество итераций алгоритма
        double constForCheck; // Для проверки критерия завершения счёта
        if (rank == 0)
            constForCheck = mod2(b) * EPSILON * EPSILON;
        char flag = 0; // 0 - продолжать счёт, 1 - счёт удачно закончен, 2 - слишком много итераций

        while (true)
        {
            // Вычисления (partRes = partA * x - partB)
            mul(partA, x, partRes, sendcountsA[rank] / N);
            sub(partRes, partB, sendcountsB[rank]);

            // Проверка на то, нужно ли завершать вычисления
            MPI_Gatherv(partRes, sendcountsB[rank], MPI_DOUBLE, res, sendcountsB, displsB, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            if (rank == 0 && mod2(res) < constForCheck)
            {
                copy(x, res);
                flag = 1;
            }
            MPI_Bcast(&flag, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
            if (flag == 1)
                break;

            // Вычисления (partRes = partX - TAU * (partA * x - partB))
            mulTAU(partRes, sendcountsB[rank]);
            subPartX(partRes, x, sendcountsB[rank], displsB[rank]);

            // Переход на новую итерацию (x_(n+1) = f(x_n))
            MPI_Allgatherv(partRes, sendcountsB[rank], MPI_DOUBLE, x, sendcountsB, displsB, MPI_DOUBLE, MPI_COMM_WORLD);

            // Проверка на количество итераций
            if ((++iters) > MAX_ITERS)
            {
                flag = 2;
                break;
            }
        }

        if (rank == 0 && flag == 2)
            std::cerr << "Too many iterations\n";
        else if (rank == 0)
        {
            const double endTime = MPI_Wtime();
            std::cout << "Total time: " << endTime - startTime << "\n";
            testAns(res);
        }
    }
    catch (const myException &e)
    {
        std::cerr << "Process " << rank << ": " << e.what() << '\n';
    }

    deleteData(A, b, partA, partB, x, res, partRes, sendcountsA, sendcountsB, displsA, displsB);
    MPI_Finalize();
    return 0;
}
