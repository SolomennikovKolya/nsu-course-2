
#include "blas_matrix.h"
#include "../openblas_x64-windows/include/openblas/cblas.h"
#include <iostream>

BlasMatrix::BlasMatrix(int size)
{
    arr_ = new float[size * size];
    size_ = size;
}

BlasMatrix::~BlasMatrix() noexcept
{
    delete[] arr_;
}

void BlasMatrix::makeZeroMatrix() noexcept
{
    for (int i = 0; i < size_ * size_; ++i)
        arr_[i] = 0;
}

void BlasMatrix::makeUnitMatrix() noexcept
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

void BlasMatrix::copy(BlasMatrix &other) const noexcept
{
    // Копирование (Размер массива, массив 1, шаг в массиве 1, массив 2, шаг в массиве 2)
    cblas_scopy(size_ * size_, arr_, 1, other.arr_, 1);
}

void BlasMatrix::print() const noexcept
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

int BlasMatrix::id(const int x, const int y) const noexcept
{
    return y * size_ + x;
}

float *BlasMatrix::operator[](int y) noexcept
{
    return arr_ + (y * size_);
}

float BlasMatrix::mod_1() const noexcept
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

float BlasMatrix::mod_inf() const noexcept
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

void BlasMatrix::transpose(BlasMatrix &res) const noexcept
{
    for (int j = 0; j < size_; ++j)
    {
        for (int i = 0; i < size_; ++i)
        {
            res.arr_[id(i, j)] = arr_[id(j, i)];
        }
    }
}

void BlasMatrix::sub(const BlasMatrix &other, BlasMatrix &res) const noexcept
{
    // Копирование (Размер массива, массив 1, шаг в массиве 1, массив 2, шаг в массиве 2)
    cblas_scopy(size_ * size_, arr_, 1, res.arr_, 1);
    // Сложение (Размер массива, множитель, массив 1, шаг в массиве 1, массив 2, шаг в массиве 2)
    cblas_saxpy(size_ * size_, -1.0, other.arr_, 1, res.arr_, 1);
}

void BlasMatrix::mult(const float alpha) noexcept
{
    // Умножение на скаляр (Размер массива, множитель, массив 1, шаг в массиве 1)
    cblas_sscal(size_ * size_, alpha, arr_, 1);
}

void BlasMatrix::mult(const BlasMatrix &other, BlasMatrix &res) const noexcept
{
    // Умножение матриц (... размеры матриц, alpha, A, локальный размер lda, B, локальный размер ldb, beta, C, локальный размер ldc)
    // C = alpha * op(A) * op(B) + beta * C
    cblas_sgemm(CblasRowMajor, CblasNoTrans, CblasNoTrans, size_, size_, size_, 1.0, arr_, size_, other.arr_, size_, 0.0, res.arr_, size_);
}

void BlasMatrix::addUnitMatrix() noexcept
{
    for (int i = 0; i < size_; ++i)
        ++arr_[id(i, i)];
}

void BlasMatrix::reverse(BlasMatrix &res, const int iterations) const noexcept
{
    BlasMatrix B(size_);
    this->transpose(B);
    B.mult(float(1) / (this->mod_1() * this->mod_inf()));

    BlasMatrix I(size_);
    I.makeUnitMatrix();

    BlasMatrix Tmp(size_);
    B.mult(*this, Tmp);

    BlasMatrix R(size_);
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
