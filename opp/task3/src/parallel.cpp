
#include "config.h"

#include <cmath>
#include <exception>
#include <iostream>
#include <omp.h>

// Для обработки ошибок в параллельной секции
enum whileFlag
{
    CONTINUE_WHILE,
    TOO_MANY_ITERATIONS,
    CORRECT_RESULT,
    WRONG_RESULT,
};

// Класс исключений для отслеживания ошибок
class myException : public std::exception
{
private:
    const char *msg_;

public:
    explicit myException(const char *msg) : msg_(msg){};
    const char *what() const noexcept override { return msg_; }
};

// Инициализация буферов (обязательно последовательный код)
inline void initData(double *&A, double *&b, double *&x, double *&res)
{
    A = (double *)malloc(sizeof(double) * (N * N));
    if (A == nullptr)
        throw myException("Malloc fail");

    b = (double *)malloc(sizeof(double) * N);
    if (b == nullptr)
        throw myException("Malloc fail");

    x = (double *)malloc(sizeof(double) * N);
    if (x == nullptr)
        throw myException("Malloc fail");

    res = (double *)malloc(sizeof(double) * N);
    if (res == nullptr)
        throw myException("Malloc fail");
}

// Заполнение буферов
inline void fillData(double *&A, double *&b, double *&x, double *&res)
{
#pragma omp for collapse(2) schedule(runtime)
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

#pragma omp for schedule(runtime)
    for (int i = 0; i < N; ++i)
        b[i] = N + 1.0;

#pragma omp for schedule(runtime)
    for (int i = 0; i < N; ++i)
        x[i] = sin(i);
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
#pragma omp for schedule(runtime)
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
#pragma omp for schedule(runtime)
    for (int i = 0; i < N; ++i)
        res[i] *= TAU;
}

// res = res - b
inline void sub(double *const &res, const double *const &b)
{
#pragma omp for schedule(runtime)
    for (int i = 0; i < N; ++i)
        res[i] -= b[i];
}

// Квадрат модуля вектора (mod2Res - общая переменная, где будет находиться ответ)
void mod2(const double *const &x, double &mod2Res)
{
#pragma omp single
    mod2Res = 0;
#pragma omp for reduction(+ : mod2Res)
    for (int i = 0; i < N; ++i)
        mod2Res += x[i] * x[i];
}

// res = x
inline void copy(const double *const &x, double *const &res)
{
#pragma omp for schedule(runtime)
    for (int i = 0; i < N; ++i)
        res[i] = x[i];
}

// Модуль для double
inline double abs(const double a)
{
    return a < 0 ? -a : a;
}

// Проверка ответа на корректность
void testAns(const double *const &res, whileFlag &flag)
{
#pragma omp for schedule(runtime)
    for (int i = 0; i < N; ++i)
    {
        if (abs(res[i] - 1.0) > CALCULATION_ERROR)
        {
#pragma omp critical
            fprintf(stderr, "res[%d] = %lf, but expected 1.0\n", i, res[i]);
            flag = WRONG_RESULT;
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
    int iters = 0;                   // Кол-во итерации цикла while
    whileFlag flag = CONTINUE_WHILE; // Для работы цикла while
    double mod2Res;                  // Переменная для вычисления функции mod2
    double constForCheck = 0;        // Константа для проверки на выход из цикла

#pragma omp parallel firstprivate(iters, flag) shared(mod2Res, constForCheck) default(shared)
    {
        fillData(A, b, x, res);

        mod2(b, mod2Res);
#pragma omp single
        constForCheck = mod2Res * EPSILON * EPSILON;

        while (flag == CONTINUE_WHILE)
        {
            mul(A, x, res);
            sub(res, b);

            mod2(res, mod2Res);
            if (mod2Res < constForCheck)
            {
                flag = CORRECT_RESULT;
                copy(x, res);
                testAns(res, flag);
            }

            if (flag == CONTINUE_WHILE)
            {

                mulTAU(res);
                sub(x, res);

                if ((++iters) > MAX_ITERS)
                    flag = TOO_MANY_ITERATIONS;
            }
        }
    }

    deleteData(A, b, x, res);
    if (flag == TOO_MANY_ITERATIONS)
        std::cerr << "Too many iterations\n";
    else if (flag == WRONG_RESULT)
        std::cerr << "Wrong result\n";

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
