
#include "intrinsic_matrix.h"
#include <intrin.h>
#include <iostream>

IntrinsicMatrix::IntrinsicMatrix(int size)
{
    arr_ = new float[size * size];
    size_ = size;
}

IntrinsicMatrix::~IntrinsicMatrix() noexcept
{
    delete[] arr_;
}

void IntrinsicMatrix::makeZeroMatrix() noexcept
{
    for (int i = 0; i < size_ * size_; ++i)
        arr_[i] = 0;
}

void IntrinsicMatrix::makeUnitMatrix() noexcept
{
    for (int j = 0; j < size_; ++j)
    {
        for (int i = 0; i < size_; ++i)
        {
            if (i == j)
                arr_[id(i, j)] = 1;
            else
                arr_[id(i, j)] = 0;
        }
    }
}

void IntrinsicMatrix::copy(IntrinsicMatrix &other) const noexcept
{
    for (int i = 0; i < size_ * size_; ++i)
        other.arr_[i] = arr_[i];
}

void IntrinsicMatrix::print() const noexcept
{
    for (int j = 0; j < size_; ++j)
    {
        for (int i = 0; i < size_; ++i)
        {
            std::cout << arr_[id(i, j)] << "\t";
        }
        std::cout << "\n";
    }
    std::cout << "\n";
}

int IntrinsicMatrix::id(const int x, const int y) const noexcept
{
    return y * size_ + x;
}

float *IntrinsicMatrix::operator[](int y) noexcept
{
    return arr_ + (y * size_);
}

float IntrinsicMatrix::mod_1() const noexcept
{
    float ans = FLT_MIN;
    for (int i = 0; i < size_; ++i)
    {
        float sum = 0;
        for (int j = 0; j < size_; ++j)
            sum += fabs(arr_[id(i, j)]);
        if (sum > ans)
            ans = sum;
    }
    return ans;
}

float IntrinsicMatrix::mod_inf() const noexcept
{
    float ans = FLT_MIN;
    for (int j = 0; j < size_; ++j)
    {
        float sum = 0;
        for (int i = 0; i < size_; ++i)
            sum += fabs(arr_[id(i, j)]);
        if (sum > ans)
            ans = sum;
    }
    return ans;
}

void IntrinsicMatrix::transpose(IntrinsicMatrix &res) const noexcept
{
    for (int j = 0; j < size_; ++j)
    {
        for (int i = 0; i < size_; ++i)
        {
            res.arr_[id(i, j)] = arr_[id(j, i)];
        }
    }
}

void IntrinsicMatrix::sub(const IntrinsicMatrix &other, IntrinsicMatrix &res) const noexcept
{
    // __m128 = 4 float в SSE
    // __m256 = 8 float в AVX
    for (int i = 0; i < size_ * size_; i += 8)
    {
        __m256 vec1 = _mm256_loadu_ps(arr_ + i);
        __m256 vec2 = _mm256_loadu_ps(other.arr_ + i);
        __m256 vecRes = _mm256_sub_ps(vec1, vec2);
        _mm256_storeu_ps(res.arr_ + i, vecRes);
    }
}

void IntrinsicMatrix::mult(const float alpha) noexcept
{
    __m256 vecAlpha = _mm256_set_ps(alpha, alpha, alpha, alpha, alpha, alpha, alpha, alpha);
    for (int i = 0; i < size_ * size_; i += 8)
    {
        __m256 vec = _mm256_load_ps(arr_ + i);
        vec = _mm256_mul_ps(vec, vecAlpha);
        _mm256_storeu_ps(arr_ + i, vec);
    }
}

void IntrinsicMatrix::mult(const IntrinsicMatrix &other, IntrinsicMatrix &res) const noexcept
{
    IntrinsicMatrix resT(size_);
    resT.makeZeroMatrix();

    IntrinsicMatrix BT(size_);
    other.transpose(BT);

    __m256 vecA, vecBT;
    for (int i = 0; i < size_; ++i)
    {
        for (int j = 0; j < size_; ++j)
        {
            for (int k = 0; k < size_; k += 8)
            {
                vecA = _mm256_load_ps(&arr_[id(k, i)]);
                vecBT = _mm256_load_ps(&BT.arr_[id(k, j)]);

                vecBT = _mm256_mul_ps(vecA, vecBT);

                // Выполняем горизонтальное сложение всех элементов вектора vecBT через параллельное сложение
                vecA = _mm256_permute2f128_ps(vecBT, vecBT, 1);
                vecBT = _mm256_add_ps(vecBT, vecA);
                vecBT = _mm256_hadd_ps(vecBT, vecBT);
                vecBT = _mm256_hadd_ps(vecBT, vecBT);

                // Извлекаем сумму из вектора
                resT.arr_[id(i, j)] += _mm256_cvtss_f32(vecBT);
            }
        }
    }

    resT.transpose(res);
}

void IntrinsicMatrix::addUnitMatrix() noexcept
{
    for (int i = 0; i < size_; ++i)
        ++arr_[id(i, i)];
}

void IntrinsicMatrix::reverse(IntrinsicMatrix &res, const int iterations) const noexcept
{
    IntrinsicMatrix B(size_);
    this->transpose(B);
    B.mult(float(1) / (this->mod_1() * this->mod_inf()));

    IntrinsicMatrix I(size_);
    I.makeUnitMatrix();

    IntrinsicMatrix Tmp(size_);
    B.mult(*this, Tmp);

    IntrinsicMatrix R(size_);
    R.makeUnitMatrix();
    I.sub(Tmp, R);

    I.copy(Tmp);
    bool flag = true;
    for (int i = 0; i < iterations - 1; ++i)
    {
        if (flag)
        {
            Tmp.mult(R, I);
            I.addUnitMatrix();
        }
        else
        {
            I.mult(R, Tmp);
            Tmp.addUnitMatrix();
        }
        flag = !flag;
    }
    if (flag)
        Tmp.mult(B, res);
    else
        I.mult(B, res);
}
