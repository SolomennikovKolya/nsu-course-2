
#ifndef UNIVERSE_H
#define UNIVERSE_H

#include "field.h"
#include <string>

const int INDENT = 1; // Отступ между полями при отрисовке

struct transitionRule
{
    bool birth[9];    // Количесва соседей для рождения клетка
    bool survival[9]; // Количесва соседей для выживания клетка
};

class Universe
{
private:
    Field field_;         // Поле
    Field nextField_;     // Промежуточное поля для того, чтобы работал nextStep
    transitionRule rule_; // Правило перехода клеточного автомата
    std::string name_;    // Имя вселенной
    int iterations_;      // Количество циклов с момента создания вселенной

    void fillTransitionRuleByDefault() noexcept;                            // Заполнение стандпртного правила перехода B3/S23
    std::string getRuleStr() const noexcept;                                // Выдаёт строчку с правилом перехода
    void inputData(std::ifstream &inp, const char *inputFilename) noexcept; // Пытается ссчитать первые 3 строчки: #Life 1.06, #N My universe, #R Bx/Sy и заполняет поля Universe
    void inputCoordinates(std::ifstream &inp) noexcept;                     // Ссчитывет координаты и заболняет field_

public:
    Universe() noexcept;  // Объявление вселенной
    ~Universe() noexcept; // Деструктор

    void init(const InitialParameters &initParam);          // Создание вселенной
    void draw() const noexcept;                             // Отрисовка вселенной
    void nextStep(unsigned int n = 1) noexcept;             // Симуляция n циклов жизни (при этом очевидно меняется поле)
    void load(const char *inputFilename);                   // Загрузить вселенную из файла
    void upload(const char *outputFilename) const noexcept; // Загрузить вселенную в файл
};

#endif
