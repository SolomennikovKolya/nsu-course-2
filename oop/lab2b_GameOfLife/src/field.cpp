
#include "field.h"
#include <chrono>
#include <intrin.h>
#include <iostream>
#include <random>

Field::Field() noexcept
{
    capacity_ = 0;
    width_ = 0;
    height_ = 0;
}

Field::~Field() noexcept
{
    delete[] field_;
}

void Field::init(int width, int height)
{
    if (width <= 0 || height <= 0)
        throw Field::exception("The size of the field must be greater than zero");
    capacity_ = width * height;
    field_ = new bool[capacity_];
    if (!field_)
        throw Field::exception("Couldn't allocate memory for Field");
    width_ = width;
    height_ = height;
}

void Field::createRandom()
{
    init(DEFAULT_FIELD_W, DEFAULT_FIELD_H);

    std::mt19937 gen(static_cast<unsigned int>(std::chrono::system_clock::now().time_since_epoch().count()));
    std::uniform_int_distribution<> dist(0, 100);

    for (int i = 0; i < capacity_; ++i)
    {
        int res = dist(gen);
        field_[i] = (res <= LIFE_CHANCE);
    }
}

void Field::draw() const noexcept
{
    std::cout << "  ";
    for (int x = 0; x < width_; ++x)
        std::cout << x % 10 << " ";
    std::cout << "\n";
    for (int y = 0; y < height_; ++y)
    {
        std::cout << y % 10 << " ";
        for (int x = 0; x < width_; ++x)
        {
            if (item(x, y))
                printf("\033[3;44;30m%c \033[0m", LIVING_CELL_CYMBOL);
            else
                printf("\033[3;44;30m. \033[0m");
        }
        std::cout << "\n";
    }
}

static int mod(int x, int base)
{
    if (x >= 0)
        return x % base;
    return x % base + base;
}

bool &Field::item(int x, int y) noexcept
{
    return field_[mod(y, height_) * width_ + mod(x, width_)];
}

const bool &Field::item(int x, int y) const noexcept
{
    return field_[mod(y, height_) * width_ + mod(x, width_)];
}

bool *Field::operator[](int y) noexcept
{
    return field_ + (mod(y, height_) * width_);
}

const bool *Field::operator[](int y) const noexcept
{
    return field_ + (mod(y, height_) * width_);
}

int Field::getWidth() const noexcept
{
    return width_;
}

int Field::getHeight() const noexcept
{
    return height_;
}

void Field::copy(Field &other)
{
    if (width_ != other.width_ || height_ != other.height_ || capacity_ != other.capacity_)
        throw Field::exception("Cannot copy field");
    for (int i = 0; i < capacity_; ++i)
        field_[i] = other.field_[i];
}
