
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

// Инициализация входных данных и вектора для записи результата
inline void initData(double *&A, double *&b, double *&x, double *&res)
{
    A = (double *)malloc(sizeof(double) * (N * N));
    if (A == nullptr)
        throw myException("Malloc fail");
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; ++j)
        {
            if (i == j)
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

    x = (double *)malloc(sizeof(double) * N);
    if (x == nullptr)
        throw myException("Malloc fail");
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
    for (int i = 0; i < N; ++i)
    {
        res[i] = 0;
        const int iN = i * N;
        for (int j = 0; j < N; ++j)
            res[i] += A[iN + j] * x[j];
    }
}

// res = res * TAU
inline void mulTAU(double *const &res)
{
    for (int i = 0; i < N; ++i)
        res[i] *= TAU;
}

// res = res - b
inline void sub(double *const &res, const double *const &b)
{
    for (int i = 0; i < N; ++i)
        res[i] -= b[i];
}

// Квадрат модуля вектора
double mod2(const double *const &x)
{
    double ans = 0;
    for (int i = 0; i < N; ++i)
        ans += x[i] * x[i];
    return ans;
}

// res = x
inline void copy(const double *const &x, double *const &res)
{
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
    for (int i = 0; i < N; ++i)
    {
        if (abs(res[i] - 1.0) > CALCULATION_ERROR)
        {
            fprintf(stderr, "res[%d] = %lf, but expected 1.0\n", i, res[i]);
            throw myException("Wrong result");
        }
    }
}

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
    // std::cout << "Iteration: " << iters << "\n";
    testAns(res);
    deleteData(A, b, x, res);

    endTime = omp_get_wtime();
    return endTime - startTime;
}

int main(int argc, char **argv)
{
    try
    {
        const double deltaTime = alg();
        std::cout << "Total time: " << deltaTime << "\n";
    }
    catch (const myException &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
