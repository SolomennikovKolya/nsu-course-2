
#include "usual_matrix.h"
#include <iostream>

UsualMatrix::UsualMatrix(int size)
{
    arr_ = new float[size * size];
    size_ = size;
}

// UsualMatrix::UsualMatrix(UsualMatrix &other)
// {
//     arr_ = new float[other.size_ * other.size_];
//     size_ = other.size_;
//     for (int i = 0; i < size_ * size_; ++i)
//         arr_[i] = other.arr_[i];
// }

UsualMatrix::~UsualMatrix() noexcept
{
    delete[] arr_;
}

void UsualMatrix::makeZeroMatrix() noexcept
{
    for (int i = 0; i < size_ * size_; ++i)
        arr_[i] = 0;
}

void UsualMatrix::makeUnitMatrix() noexcept
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

void UsualMatrix::copy(UsualMatrix &other) const noexcept
{
    for (int i = 0; i < size_ * size_; ++i)
        other.arr_[i] = arr_[i];
}

void UsualMatrix::print() const noexcept
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

int UsualMatrix::id(const int x, const int y) const noexcept
{
    return y * size_ + x;
}

float *UsualMatrix::operator[](int y) noexcept
{
    return arr_ + (y * size_);
}

float UsualMatrix::mod_1() const noexcept
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

float UsualMatrix::mod_inf() const noexcept
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

void UsualMatrix::transpose(UsualMatrix &res) const noexcept
{
    for (int j = 0; j < size_; ++j)
    {
        for (int i = 0; i < size_; ++i)
        {
            res.arr_[id(i, j)] = arr_[id(j, i)];
        }
    }
}

// void UsualMatrix::add(UsualMatrix &other) const noexcept
// {
//     for (int i = 0; i < size_ * size_; ++i)
//         other.arr_[i] += arr_[i];
// }

// void UsualMatrix::add(const UsualMatrix &other, UsualMatrix &res) const noexcept
// {
//     for (int i = 0; i < size_ * size_; ++i)
//         res.arr_[i] = arr_[i] + other.arr_[i];
// }

void UsualMatrix::sub(const UsualMatrix &other, UsualMatrix &res) const noexcept
{
    for (int i = 0; i < size_ * size_; ++i)
        res.arr_[i] = arr_[i] - other.arr_[i];
}

void UsualMatrix::mult(const float alpha) noexcept
{
    for (int i = 0; i < size_ * size_; ++i)
        arr_[i] *= alpha;
}

void UsualMatrix::mult(const UsualMatrix &other, UsualMatrix &res) const noexcept
{
    res.makeZeroMatrix();
    for (int j = 0; j < size_; j++)
    {
        for (int i = 0; i < size_; i++)
        {
            for (int k = 0; k < size_; ++k)
            {
                res.arr_[id(k, j)] += arr_[id(i, j)] * other.arr_[id(k, i)];
            }
        }
    }
}

void UsualMatrix::addUnitMatrix() noexcept
{
    for (int i = 0; i < size_; ++i)
        ++arr_[id(i, i)];
}

void UsualMatrix::reverse(UsualMatrix &res, const int iterations) const noexcept
{
    UsualMatrix B(size_);
    // B = A^T
    this->transpose(B);
    // B = A^T / (mod_1 * mod_inf)
    B.mult(float(1) / (this->mod_1() * this->mod_inf()));

    UsualMatrix I(size_);
    // I = единичная матрица
    I.makeUnitMatrix();

    UsualMatrix Tmp(size_);
    // Tmp = B * A
    B.mult(*this, Tmp);

    UsualMatrix R(size_);
    // R = I
    R.makeUnitMatrix();
    // R = I - Tmp = I - BA
    I.sub(Tmp, R);

    // Tmp = I
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
