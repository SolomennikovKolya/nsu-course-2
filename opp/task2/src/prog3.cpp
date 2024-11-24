
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

// Инициализация входных данных и вектора для записи результата в корневом процессе
void initData(double *&A, double *&b, double *&x, double *&res, const int N1)
{
    A = (double *)malloc(sizeof(double) * (N1 * N));
    if (A == nullptr)
        throw myException("Malloc fail");
    for (int i = 0; i < N1; ++i)
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

    b = (double *)malloc(sizeof(double) * N1);
    if (b == nullptr)
        throw myException("Malloc fail");
    for (int i = 0; i < N; ++i)
        b[i] = N + 1.0;
    for (int i = N; i < N1; ++i)
        b[i] = 0.0;

    x = (double *)malloc(sizeof(double) * N);
    if (x == nullptr)
        throw myException("Malloc fail");
    for (int i = 0; i < N; ++i)
        x[i] = sin(i);

    res = (double *)malloc(sizeof(double) * N1);
    if (res == nullptr)
        throw myException("Malloc fail");
}

// Инициализация буферов для не корневых процессов
void partInitData(double *&A, double *&b, double *&x, double *&res, const int partSize)
{
    A = (double *)malloc(sizeof(double) * (partSize * N));
    if (A == nullptr)
        throw myException("Malloc fail");

    b = (double *)malloc(sizeof(double) * partSize);
    if (b == nullptr)
        throw myException("Malloc fail");

    x = (double *)malloc(sizeof(double) * N);
    if (x == nullptr)
        throw myException("Malloc fail");

    res = (double *)malloc(sizeof(double) * partSize);
    if (res == nullptr)
        throw myException("Malloc fail");
}

// Освобождение ресурсов
void deleteData(double *&A, double *&b, double *&x, double *&res)
{
    free(A);
    free(b);
    free(x);
    free(res);
}

// res = A * x; вычисляет только первые partSize элементов
void mul(const double *const &A, const double *const &x, double *const &res, const int partSize)
{
    for (int i = 0; i < partSize; ++i)
    {
        res[i] = 0;
        const int iN = i * N;
        for (int j = 0; j < N; ++j)
            res[i] += A[iN + j] * x[j];
    }
}

// res = res * TAU; вычисляет только первые partSize элементов
void mulTAU(double *const &res, const int partSize)
{
    for (int i = 0; i < partSize; ++i)
        res[i] *= TAU;
}

// res = res - b; вычисляет только первые partSize элементов
void sub(double *const &res, const double *const &b, const int partSize)
{
    for (int i = 0; i < partSize; ++i)
        res[i] -= b[i];
}

// Квадрат модуля вектора; вычисляет только для первых len элементов
double mod2(const double *const &x, const int len)
{
    double ans = 0;
    for (int i = 0; i < len; ++i)
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

    double *A, *b, *x, *res;
    int partSize = N / size;
    int N1 = N;
    if (N % size != 0)
    {
        // Расширение векторов для случая, если N не делится нацело на количество процессов
        // Это нужно, чтобы данные равномерно распределялись по всем процессам
        // Реальные размеры векторов в корневом процессе будут:    size(A) = N1 * N,       size(b) = N1,       size(x) = N, size(res) = N1
        // Реальные размеры векторов в не корневом процессе будут: size(A) = partSize * N, size(b) = partSize, size(x) = N, size(res) = partSize
        N1 += (size - N % size);
        partSize++;
    }
    double startTime, endTime;

    try
    {
        // Начальная инициализация данных
        if (rank == 0)
        {
            initData(A, b, x, res, N1);
            startTime = MPI_Wtime();
        }
        else
            partInitData(A, b, x, res, partSize);

        // Раздача данных не корневым процессам
        MPI_Scatter(A, partSize * N, MPI_DOUBLE, A, partSize * N, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Scatter(b, partSize, MPI_DOUBLE, b, partSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
        MPI_Bcast(x, N, MPI_DOUBLE, 0, MPI_COMM_WORLD);

        int iters;            // Количество итераций алгоритма
        double constForCheck; // Для проверки критерия завершения счёта
        if (rank == 0)
        {
            iters = 0;
            constForCheck = mod2(b, N) * EPSILON * EPSILON;
        }
        char flag = 0; // 0 - продолжить счёт, 1 - счёт успешно завершён, 2 - слишком много итераций

        while (true)
        {
            // Вычисления
            mul(A, x, res, partSize);
            sub(res, b, partSize);

            // Проверка на то, нужно ли завершать вычисления
            MPI_Gather(res, partSize, MPI_DOUBLE, res, partSize, MPI_DOUBLE, 0, MPI_COMM_WORLD);
            if (rank == 0 && mod2(res, N) < constForCheck)
            {
                copy(x, res);
                flag = 1;
            }
            MPI_Bcast(&flag, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
            if (flag == 1)
                break;

            // Вычисления
            mulTAU(res, partSize);
            for (int i = 0; i < partSize; ++i)
                res[i] = x[partSize * rank + i] - res[i];
            MPI_Allgather(res, partSize, MPI_DOUBLE, x, partSize, MPI_DOUBLE, MPI_COMM_WORLD);

            // Проверка на количество итераций
            if (rank == 0 && (++iters) > MAX_ITERS)
                flag = 2;
            MPI_Bcast(&flag, 1, MPI_CHAR, 0, MPI_COMM_WORLD);
            if (flag == 2)
                break;
        }

        if (flag == 2)
        {
            if (rank == 0)
                std::cerr << "Too many iterations\n";
        }
        else
        {
            endTime = MPI_Wtime();
            std::cout << "Total time: " << endTime - startTime << "\n";
            testAns(res);
        }
    }
    catch (const myException &e)
    {
        std::cerr << "Process " << rank << ": " << e.what() << '\n';
    }

    deleteData(A, b, x, res);
    MPI_Finalize();
    return 0;
}
