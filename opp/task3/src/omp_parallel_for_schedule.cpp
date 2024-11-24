
#include "config.h"

#include <cmath>
#include <exception>
#include <iostream>
#include <omp.h>

// Класс исключений для отслеживания ошибок
class myException : public std::exception
{
private:
    const char *msg_;

public:
    explicit myException(const char *msg) : msg_(msg){};
    const char *what() const noexcept override { return msg_; }
};

// Инициализация буферов
inline void initData(double *&A, double *&b, double *&x, double *&res)
{
    A = (double *)malloc(sizeof(double) * (N * N));
    if (A == nullptr)
        throw myException("Malloc fail");
#pragma omp parallel for collapse(2) schedule(runtime)
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
#pragma omp parallel for schedule(runtime)
    for (int i = 0; i < N; ++i)
        b[i] = N + 1.0;

    x = (double *)malloc(sizeof(double) * N);
    if (x == nullptr)
        throw myException("Malloc fail");
#pragma omp parallel for schedule(runtime)
    for (int i = 0; i < N; ++i)
        x[i] = sin(i);

    res = (double *)malloc(sizeof(double) * N);
    if (res == nullptr)
        throw myException("Malloc fail");
}

// Освобождение ресурсов
inline void deleteData(double *&A, double *&b, double *&x, double *&res)
{
    free(A);
    free(b);
    free(x);
    free(res);
}

// res = A * x
inline void mul(const double *const &A, const double *const &x, double *const &res)
{
#pragma omp parallel for schedule(runtime)
    for (int i = 0; i < N; ++i)
    {
        res[i] = 0;
        for (int j = 0; j < N; ++j)
            res[i] += A[i * N + j] * x[j];
    }
}

// res = res * TAU
inline void mulTAU(double *const &res)
{
#pragma omp parallel for schedule(runtime)
    for (int i = 0; i < N; ++i)
        res[i] *= TAU;
}

// res = res - b
inline void sub(double *const &res, const double *const &b)
{
#pragma omp parallel for schedule(runtime)
    for (int i = 0; i < N; ++i)
        res[i] -= b[i];
}

// Квадрат модуля вектора
double mod2(const double *const &x)
{
    double ans = 0;
#pragma omp parallel for reduction(+ : ans)
    for (int i = 0; i < N; ++i)
        ans += x[i] * x[i];
    return ans;
}

// res = x
inline void copy(const double *const &x, double *const &res)
{
#pragma omp parallel for schedule(runtime)
    for (int i = 0; i < N; ++i)
        res[i] = x[i];
}

// Модуль для double
inline double abs(const double a)
{
    return a < 0 ? -a : a;
}

// Проверка ответа на корректность
inline void testAns(const double *const &res)
{
#pragma omp parallel for schedule(runtime)
    for (int i = 0; i < N; ++i)
    {
        if (abs(res[i] - 1.0) > CALCULATION_ERROR)
        {
#pragma omp critical
            fprintf(stderr, "res[%d] = %lf, but expected 1.0\n", i, res[i]);
        }
    }
}

// Алгоритм для нахождения решения системы линейных алгебраических уравнений. Возвращает время работы алгоритма
inline double alg()
{
    double startTime, endTime;
    startTime = omp_get_wtime();

    double *A = nullptr, *b = nullptr, *x = nullptr, *res = nullptr;
    initData(A, b, x, res);
    int iters = 0;
    const double constForCheck = mod2(b) * EPSILON * EPSILON;
    while (true)
    {
        mul(A, x, res);
        sub(res, b);

        if (mod2(res) < constForCheck)
        {
            copy(x, res);
            break;
        }

        mulTAU(res);
        sub(x, res);

        if ((++iters) > MAX_ITERS)
            throw myException("Too many iterations");
    }
    testAns(res);
    deleteData(A, b, x, res);

    endTime = omp_get_wtime();
    return endTime - startTime;
}

int main(int argc, char **argv)
{
    const int TIME_TEST_ITERS = 5; // Количество повторений всего алгоритма
    double minTime = 10000, maxTime = 0, sumTime = 0;

    try
    {
        for (int i = 0; i < TIME_TEST_ITERS; ++i)
        {
            const double deltaTime = alg();
            sumTime += deltaTime;
            minTime = std::min(minTime, deltaTime);
            maxTime = std::max(maxTime, deltaTime);
            std::cout << "Total time: " << deltaTime << "\n";
        }
        std::cout << "Min time: " << minTime << "\n";
        std::cout << "Max time: " << maxTime << "\n";
        std::cout << "Average time: " << sumTime / TIME_TEST_ITERS << "\n";

        // const double deltaTime = alg();
        // std::cout << "Total time: " << deltaTime << "\n";
    }
    catch (const myException &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
