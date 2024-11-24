
#include <algorithm>
#include <iostream>
#include <limits>
#include <math.h>

#define N 2 // 2048
#define M 1000

using namespace std;

// Печать матрицы в консоль
void printMatrix(const float *a)
{
    for (int y = 0; y < N; ++y)
    {
        for (int x = 0; x < N; ++x)
            cout << a[y * N + x] << "\t";
        cout << "\n";
    }
    cout << "\n";
}

// b = a
void copy(const float *a, float *b)
{
    for (int i = 0; i < N * N; ++i)
        b[i] = a[i];
}

// Заполнить нулями
void fillInWithZeros(float *a)
{
    for (int i = 0; i < N * N; ++i)
        a[i] = 0;
}

// Максимальная сумма столбцов
float mod_1(float *a)
{
    float ans = FLT_MIN;
    for (int x = 0; x < N; ++x)
    {
        float sum = 0;
        for (int y = 0; y < N; ++y)
            sum += fabs(a[y * N + x]);
        ans = max(ans, sum);
    }
    return ans;
}

// максимальная сумма строк
float mod_inf(float *a)
{
    float ans = FLT_MIN;
    for (int y = 0; y < N; ++y)
    {
        float sum = 0;
        for (int x = 0; x < N; ++x)
            sum += fabs(a[y * N + x]);
        ans = max(ans, sum);
    }
    return ans;
}

// a^T = b
void transpose(const float *a, float *b)
{
    for (int y = 0; y < N; ++y)
    {
        for (int x = y + 1; x < N; ++x)
        {
            b[y * N + x] = a[x * N + y];
            b[x * N + y] = a[y * N + x];
        }
    }
    for (int i = 0; i < N; ++i)
        b[i * N + i] = a[i * N + i];
}

// a + b = b
void add_in_place(const float *a, float *b)
{
    for (int i = 0; i < N * N; ++i)
        b[i] += a[i];
}

// res = a + b
// void add(const float *a, const float *b, float *res)
// {
//     for (int i = 0; i < N * N; ++i)
//         res[i] = a[i] + b[i];
// }

// res = a - b
// void sub(const float *a, const float *b, float *res)
// {
//     for (int i = 0; i < N * N; ++i)
//         res[i] = a[i] - b[i];
// }

// a = a * num
void multBuNumber(float *a, const float num)
{
    for (int i = 0; i < N * N; ++i)
        a[i] *= num;
}

// res = a * b
void multiply(const float *a, const float *b, float *res)
{
    fillInWithZeros(res);
    for (int i = 0; i < N; i++)
        for (int j = 0; j < N; j++)
            for (int k = 0; k < N; ++k)
                res[N * i + k] += a[N * i + j] * b[N * j + k];
}

// Обратная матрица к матрице a
void reverse(float *A, float *Res)
{
    if (M == 0)
    {
        fillInWithZeros(Res);
        return;
    }
    else if (M == 1)
    {
        fillInWithZeros(Res);
        for (int i = 0; i < N; ++i)
            Res[i * N + i] = 1;
        return;
    }

    // B = transposed(A) / (mod_1(A) + mod_inf(A))
    float *B = new float[N * N];
    transpose(A, B);
    const float A_mod_1 = mod_1(A);
    const float A_mod_inf = mod_inf(A);
    multBuNumber(B, float(1) / (A_mod_1 + A_mod_inf));

    // I = единичная матрица
    float *I = new float[N * N];
    fillInWithZeros(I);
    for (int i = 0; i < N; ++i)
        I[i * N + i] = 1;

    // R = I - BA
    float *R = new float[N * N];
    multiply(B, A, R);
    multBuNumber(R, -1);
    add_in_place(I, R);

    copy(I, Res);         // Res = I
    add_in_place(R, Res); // Res = I + R
    float *R_pow = new float[N * N];
    copy(R, R_pow); // R_pow = R
    for (int i = 2; i < M; i++)
    {
        multiply(R_pow, R, I);    // I = R_pow * R
        copy(I, R_pow);           // R_pow = I = R_pow * R
        add_in_place(R_pow, Res); // Res = Res + R_pow
    }
    multiply(Res, B, I);
    copy(I, Res);

    cout << "Reversed matrix: \n";
    printMatrix(Res);
    multiply(Res, A, I);
    cout << "Unit matrix: \n";
    printMatrix(I);

    delete[] B;
    delete[] I;
    delete[] R;
    // delete[] R_pow;
}

int main()
{
    float *A = new float[N * N];
    // for (int i = 0; i < N * N; ++i)
    //     A[i] = static_cast<float>(i);
    A[0] = 1;
    A[1] = 0;
    A[2] = 0;
    A[3] = 1;
    cout << "Matrix: \n";
    printMatrix(A);
    float *Res = new float[N * N];

    reverse(A, Res);

    delete[] A;
    delete[] Res;
    return 0;
}