
#ifndef INTRINSIC_MATRIX_H
#define INTRINSIC_MATRIX_H

class IntrinsicMatrix
{
private:
    float *arr_;
    int size_;

    // Возвращает индекс элемента в матрице с координатами (x, y)
    int id(int x, int y) const noexcept;

public:
    // Создаёт матрицу размером size * size
    IntrinsicMatrix(int size);
    // Деструктор
    ~IntrinsicMatrix() noexcept;

    // Заполняет матрицу нулями
    void makeZeroMatrix() noexcept;
    // Заполняет элементы так, чтобы получилась единична матрица
    void makeUnitMatrix() noexcept;
    // Копирование this в other
    void copy(IntrinsicMatrix &other) const noexcept;
    // Выводит матрицу в консоль
    void print() const noexcept;

    // Для обращения вида: matrix[y][x]
    float *operator[](int y) noexcept;

    // Максимальная сумма столбцов
    float mod_1() const noexcept;
    // Максимальная сумма строк
    float mod_inf() const noexcept;

    // Транспонирование: this^T = res
    void transpose(IntrinsicMatrix &res) const noexcept;
    // Добавить единичную матрицу
    void addUnitMatrix() noexcept;
    // Разница: this - other = res
    void sub(const IntrinsicMatrix &other, IntrinsicMatrix &res) const noexcept;
    // Умножение на число: this * alpha = this
    void mult(const float alpha) noexcept;
    // Умножение матриц: this * other = res
    void mult(const IntrinsicMatrix &other, IntrinsicMatrix &res) const noexcept;
    // Обращение маорицы: this^(-1) = res. Точность зависит от количества членов в ряде (iterations)
    void reverse(IntrinsicMatrix &res, const int iterations) const noexcept;
};

#endif
