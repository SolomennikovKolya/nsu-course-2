
#ifndef INITIAL_PARAMETERS_H
#define INITIAL_PARAMETERS_H

#include <string>

enum GameMode
{
    OnlineWithRandomField,
    OnlineWithFieldFromFile,
    Offline,
    Tutorial
};

class InitialParameters
{
private:
    GameMode gameMode_;
    std::string inputFilename_;  // Нужно только для Offline или OnlineWithFieldFromFile режима
    std::string outputFilename_; // Нужно только для Offline режима
    int iterations_;             // Нужно только для Offline режима

public:
    InitialParameters(int argc, char **argv);          // Обрабатывает входные данные программы
    InitialParameters(const InitialParameters &other); // Инициализация через other
    ~InitialParameters() noexcept = default;

    GameMode getGameMode() const noexcept; // Получить GameMode (0 = OnlineWithRandomField, 1 = OnlineWithFieldFromFile, 2 = Offline, 3 = Tutorial)
    const char *getInputFilename() const;  // Получить имя входного файла
    const char *getOutputFilename() const; // Получить имя выходного файла
    int getIterations() const;             // Получить количество итераций

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
