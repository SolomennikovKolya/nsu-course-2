#include <cfloat>
#include <cmath>
#include <ctime>
#include <iostream>
#include <xmmintrin.h>

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

    for (int i = 0; i < N; i++) // rows
    {
        sum_row = 0;
        sum_column = 0;

        for (int j = 0; j < N; j++) // columns
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
    // Индекс для обхода массивов с шагом 4 (размер __m128)
    for (int i = 0; i < N * N; i += 4)
    {
        // Загрузка 4 значений из addend1 и addend2
        __m128 vec_addend1 = _mm_loadu_ps(&addend1[i]);
        __m128 vec_addend2 = _mm_loadu_ps(&addend2[i]);

        // SIMD-операция сложения
        __m128 vec_result = _mm_add_ps(vec_addend1, vec_addend2);

        // Сохранение результата в result
        _mm_storeu_ps(&result[i], vec_result);
    }
}

void substract(const float *minuend, const float *subtrahend, float *result)
{
    // Индекс для обхода массивов с шагом 4 (размер __m128)
    for (int i = 0; i < N * N; i += 4)
    {
        // Загрузка 4 значений из minuend и subtrahend
        __m128 vec_minuend = _mm_loadu_ps(&minuend[i]);
        __m128 vec_subtrahend = _mm_loadu_ps(&subtrahend[i]);

        // SIMD-операция вычитания
        __m128 vec_result = _mm_sub_ps(vec_minuend, vec_subtrahend);

        // Сохранение результата в result
        _mm_storeu_ps(&result[i], vec_result);
    }
}

void copy(float *it, const float *other)
{
    for (int i = 0; i < N * N; i++)
        it[i] = other[i];
}

void fill_B(const float *matrix, float *B)
{
    float A_1, A_infinity;
    get_norms(A_1, A_infinity, matrix);

    __m128 vec_A_1 = _mm_set1_ps(A_1);
    __m128 vec_A_infinity = _mm_set1_ps(A_infinity);

    // Векторизированное умножение матрицы на скаляр
    for (int i = 0; i < N; ++i)
    {
        for (int j = 0; j < N; j += 4)
        {
            // Загрузка 4 значений из matrix
            __m128 vec_matrix = _mm_load_ps(&matrix[j * N + i]);

            // Деление каждого элемента на (A_1 * A_infinity)
            __m128 vec_result = _mm_div_ps(vec_matrix, _mm_mul_ps(vec_A_1, vec_A_infinity));

            // Сохранение результата в B
            _mm_storeu_ps(&B[N * i + j], vec_result);
        }
    }
}

void fill_I(float *I)
{
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            I[N * i + j] = (float)(i == j);
}

void multiply(const float *multiplier1, const float *multiplier2, float *result)
{
    __m128 *m128_result;
    const __m128 *m128_multiplier2;
    m128_result = (__m128 *)result;
    m128_multiplier2 = (const __m128 *)multiplier2;
    __m128 m128_multiplier1;
    __m128 tmp;

    for (int i = 0; i < N * N / 4; ++i)
        m128_result[i] = _mm_setzero_ps();

    for (int i = 0; i < N; ++i)
        for (int j = 0; j < N; ++j)
        {
            m128_multiplier1 = _mm_set1_ps(multiplier1[N * i + j]);
            for (int k = 0; k < N / 4; ++k)
            {
                tmp = _mm_mul_ps(m128_multiplier1,
                                 m128_multiplier2[N * j / 4 + k]);
                m128_result[N * i / 4 + k] = _mm_add_ps(m128_result[N * i / 4 + k],
                                                        tmp);
            }
        }
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

void Print(const float *matrix)
{
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
#if 1
    auto *identity_matrix = new float[N * N];
    multiply(matrix, result, identity_matrix); // проверка корректности обращения
    Print(identity_matrix);
    delete[] identity_matrix;
#endif
    delete[] matrix;
    delete[] result;

    std::cout << "Time with manual vectorization: " << get_elapsed_time(time_range) << " sec." << std::endl;

    return 0;
}
