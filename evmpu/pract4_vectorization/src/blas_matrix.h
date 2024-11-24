
#ifndef BLAS_MATRIX_H
#define BLAS_MATRIX_H

class BlasMatrix
{
private:
    float *arr_;
    int size_;

    // Возвращает индекс элемента в матрице с координатами (x, y)
    int id(int x, int y) const noexcept;

public:
    // Создаёт матрицу размером size * size
    BlasMatrix(int size);
    // Деструктор
    ~BlasMatrix() noexcept;

    // Заполняет матрицу нулями
    void makeZeroMatrix() noexcept;
    // Заполняет элементы так, чтобы получилась единична матрица
    void makeUnitMatrix() noexcept;
    // Копирование this в other
    void copy(BlasMatrix &other) const noexcept;
    // Выводит матрицу в консоль
    void print() const noexcept;

    // Для обращения вида: matrix[y][x]
    float *operator[](int y) noexcept;

    // Максимальная сумма столбцов
    float mod_1() const noexcept;
    // Максимальная сумма строк
    float mod_inf() const noexcept;

    // Транспонирование: this^T = res
    void transpose(BlasMatrix &res) const noexcept;
    // Добавить единичную матрицу
    void addUnitMatrix() noexcept;
    // Разница: this - other = res
    void sub(const BlasMatrix &other, BlasMatrix &res) const noexcept;
    // Умножение на число: this * alpha = this
    void mult(const float alpha) noexcept;
    // Умножение матриц: this * other = res
    void mult(const BlasMatrix &other, BlasMatrix &res) const noexcept;
    // Обращение маорицы: this^(-1) = res. Точность зависит от количества членов в ряде (iterations)
    void reverse(BlasMatrix &res, const int iterations) const noexcept;
};

#endif
