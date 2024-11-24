#include <cblas.h>
#include <ctime>
#include <iostream>
#include <cmath>
#include <cfloat>

#define N 2
#define M 100000

struct TimeRange
{
    timespec start, end;
};

double get_elapsed_time(const TimeRange &time_range)
{
    return (time_range.end.tv_sec - time_range.start.tv_sec) + 1e-9 * (time_range.end.tv_nsec - time_range.start.tv_nsec);
}

void get_norms(float &A_1, float &A_infinity, const float *matrix)
{
    A_1 = FLT_MIN;
    A_infinity = FLT_MIN;
    float sum_row = 0;
    float sum_column = 0;

    for (int i = 0; i < N; i++)
    {
        sum_row = 0;
        sum_column = 0;

        for (int j = 0; j < N; j++)
        {
            sum_row += std::fabs(matrix[N * i + j]);
            sum_column += std::fabs(matrix[j * N + i]);
        }

        if (sum_row > A_1)
            A_1 = sum_row;
        if (sum_column > A_infinity)
            A_infinity = sum_column;
    }
}

void addition(const float *addend1, const float *addend2, float *result)
{
    //cblas_scopy(N * N, addend1, 1, result, 1);
    cblas_saxpy(N * N, 1, addend2, 1, result, 1);
}

void substract(const float *minuend, const float *subtrahend, float *result)
{
    cblas_scopy(N * N, minuend, 1, result, 1);
    cblas_saxpy(N * N, -1.0, subtrahend, 1, result, 1);
}

void copy(float *dest, const float *src)
{
    for (int i = 0; i < N * N; i++)
        dest[i] = src[i];
}

void fill_B(const float *matrix, float *B)
{
    float A_1, A_infinity;
    get_norms(A_1,
              A_infinity,
              matrix);

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            B[N * i + j] = matrix[j * N + i] / (A_1 * A_infinity);
}

void fill_I(float *I)
{
    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
            I[N * i + j] = (float)(i == j);
}

void multiply(const float *multiplier1, const float *multiplier2, float *result)
{
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, N, N, N, 1.0, multiplier1, N, multiplier2, N, 0.0, result, N);
}

void inverse(const float *matrix, float *result)
{
    auto *B = new float[N * N];
    auto *I = new float[N * N];
    auto *tmp = new float[N * N];
    auto *R = new float[N * N];
    bool flag = true;

    fill_B(matrix, B);
    fill_I(I);
    multiply(B, matrix, tmp); // B * matrix
    substract(I, tmp, R);     // I - B * matrix
    addition(I, R, tmp);      // I + (I - B * matrix)
    copy(result, R);

    // Итеративное вычисление обратной матрицы
    for (int i = 2; i < M; i++)
    {
        multiply(flag ? result : I, R, flag ? I : result); // result * R или I * R
        addition(tmp, flag ? I : result, tmp);             // tmp + (result * R) или tmp + (I * R)
        flag = !flag;
    }

    multiply(tmp, B, result);

    delete[] I;
    delete[] B;
    delete[] tmp;
    delete[] R;
}

void print(const float *matrix)
{
    // Вывод матрицы в консоль
    for (int i = 0; i < N; i++)
    {
        for (int j = 0; j < N; j++)
            std::cout << matrix[N * i + j] << " ";

        std::cout << std::endl;
    }
}

int main()
{
    // srandom(time(nullptr));
    auto *matrix = new float[N * N];
    auto *result = new float[N * N];

    for (int i = 0; i < N * N; i++)
    {
        matrix[i] = (float)i;
        // matrix[i] *= (random() % 2) ? 1 : -1;
        result[i] = 0;
    }

    // float A_1, A_infinity;
    // get_norms(A_1, A_infinity, matrix);
    // std::cout << "A_1 check: " << A_1 << std::endl;
    // std::cout << "A_infinity check: " << A_infinity << std::endl;

    // Замер времени начала обращения матрицы
    TimeRange time_range;
    clock_gettime(CLOCK_MONOTONIC_RAW, &time_range.start);
    inverse(matrix, result);
    clock_gettime(CLOCK_MONOTONIC_RAW, &time_range.end);
#if 0
    auto *identity_matrix = new float[N * N];
    multiply(matrix, result, identity_matrix); // проверка корректности обращения
    print(identity_matrix);
    delete[] identity_matrix;
#endif
    delete[] matrix;
    delete[] result;
    std::cout << "Time with BLAS: " << get_elapsed_time(time_range) << " sec." << std::endl;

    return 0;
}
