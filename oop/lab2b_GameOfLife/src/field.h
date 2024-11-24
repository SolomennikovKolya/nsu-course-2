
#ifndef FIELD_H
#define FIELD_H

#include "initial_parameters.h"
#include <fstream>
#include <string>

// Для случайного поля:
const int DEFAULT_FIELD_W = 10;
const int DEFAULT_FIELD_H = 10;
const int LIFE_CHANCE = 20; // Шанс, что клетка будет живой (в процентах)

// Для поля из файла
const int MIN_FIELD_W = 10;
const int MIN_FIELD_H = 10;

// Будет использоваться при отрисовке поля (2 - смайлик, 3 - сердечко, 9 - круг, 15 - солнце)
const char LIVING_CELL_CYMBOL = 3;

class Field
{
private:
    bool *field_;  // Двумерное поле (на самом деле одномерное в памяти)
    int capacity_; // Длина аллоцированной памяти (width_ * height_)
    int width_;    // Ширина поля
    int height_;   // Длина поля

    // bool inputData(std::ifstream &inp, std::string &line); // Пытается ссчитать первые 3 строчки: #Life 1.06, #N My universe, #R Bx/Sy

public:
    Field() noexcept;  // Объявление поля
    ~Field() noexcept; // Деструктор

    void init(int width, int height); // Выделяет помять для поля размером width * height
    void createRandom();              // Создать случайне поле
    void copy(Field &other);          // Копирует поле other (Оба поля должны быть инициализированны и быть одинакового размера)
    void draw() const noexcept;       // Вывод поля в консоль

    bool &item(int x, int y) noexcept; // Доступ к клетке поля по координатам (берёт координаты по модулю размера поля)
    const bool &item(int x, int y) const noexcept;
    bool *operator[](int y) noexcept; // Указатель на y-овую строку (обращение field[y][x] эквивалентно field.item(x, y), если x >= 0)
    const bool *operator[](int y) const noexcept;

    int getWidth() const noexcept;  // Ширина поля
    int getHeight() const noexcept; // Высота поля

    class exception : public std::exception
    {
    private:
        const char *message_;

    public:
        explicit exception(const char *message) : message_(message) {}
        const char *what() const noexcept override { return message_; }
    };
};

#endif
