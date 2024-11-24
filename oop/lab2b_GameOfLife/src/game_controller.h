
#ifndef GAME_CONTROLLER_H
#define GAME_CONTROLLER_H

#include "initial_parameters.h"
#include "universe.h"

class GameController
{
private:
    GameMode gameMode_; // Режим игры
    bool inProcess_;    // true если игра запущенна

public:
    GameController() noexcept;  // Объявление игрового контроллера
    ~GameController() noexcept; // Деструктор

    void processOfflineMode(const InitialParameters &initParam, Universe &world); // Симуляция для Offline режима

    // start, draw, update, finish работают только для OnlineWithRandomField или OnlineWithFieldFromFile режимов
    void start(const InitialParameters &initParam, Universe &world); // Старт игры, инициализация игрового мира
    void draw(Universe &world);                                      // Рисует текущее состояние поля в консоле
    void update(Universe &world);                                    // Обновляет состояние игры. Ждёт cin внутри. Автоматичести освобождает память, если после обновления игра закончилась

    bool inProcess() const noexcept; // true если игра запущенна
};

#endif
