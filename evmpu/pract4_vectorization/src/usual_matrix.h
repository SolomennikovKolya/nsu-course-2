
#ifndef USUAL_MATRIX_H
#define USUAL_MATRIX_H

class UsualMatrix
{
private:
    float *arr_;
    int size_;

    // Возвращает индекс элемента в матрице с координатами (x, y)
    int id(int x, int y) const noexcept;

public:
    // Создаёт матрицу размером size * size
    UsualMatrix(int size);
    // Деструктор
    ~UsualMatrix() noexcept;

    // Заполняет матрицу нулями
    void makeZeroMatrix() noexcept;
    // Заполняет элементы так, чтобы получилась единична матрица
    void makeUnitMatrix() noexcept;
    // Копирование this в other
    void copy(UsualMatrix &other) const noexcept;
    // Выводит матрицу в консоль
    void print() const noexcept;

    // Для обращения вида: matrix[y][x]
    float *operator[](int y) noexcept;

    // Максимальная сумма столбцов
    float mod_1() const noexcept;
    // Максимальная сумма строк
    float mod_inf() const noexcept;

    // Транспонирование: this^T = res
    void transpose(UsualMatrix &res) const noexcept;
    // Добавить единичную матрицу
    void addUnitMatrix() noexcept;
    // Разница: this - other = res
    void sub(const UsualMatrix &other, UsualMatrix &res) const noexcept;
    // Умножение на число: this * alpha = this
    void mult(const float alpha) noexcept;
    // Умножение матриц: this * other = res
    void mult(const UsualMatrix &other, UsualMatrix &res) const noexcept;
    // Обращение маорицы: this^(-1) = res. Точность зависит от количества членов в ряде (iterations)
    void reverse(UsualMatrix &res, const int iterations) const noexcept;
};

#endif
