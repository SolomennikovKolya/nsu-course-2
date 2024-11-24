
#include "game_controller.h"
#include "initial_parameters.h"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

enum CommandFromLine
{
    DumpCommand,
    TickCommand,
    ExitCommand,
    HelpCommand,
    WrongCommand
};

GameController::GameController() noexcept
{
    inProcess_ = false;
}

GameController::~GameController() noexcept {}

// Оффлайн режим
void GameController::processOfflineMode(const InitialParameters &initParam, Universe &world)
{
    gameMode_ = initParam.getGameMode();
    world.load(initParam.getInputFilename());
    world.nextStep(initParam.getIterations());
    world.upload(initParam.getOutputFilename());
}

// Старт
void GameController::start(const InitialParameters &initParam, Universe &world)
{
    gameMode_ = initParam.getGameMode();
    world.init(initParam);
    inProcess_ = true;
}

// Отрисовка
void GameController::draw(Universe &world)
{
    world.draw();
}

static void splitLine(std::string line, int &argc, std::vector<std::string> &argv)
{
    std::istringstream iss(line);
    std::string cur = "";
    while (iss >> cur)
    {
        argc++;
        argv.push_back(cur);
        cur = "";
    }
}

static CommandFromLine recognizeCommandFromLine(std::string line, std::string &res)
{
    int argc = 0;
    std::vector<std::string> argv;
    splitLine(line, argc, argv);

    if (argc == 2 && argv[0] == "dump" && argv[1].length() > 2)
    {
        res = argv[1].substr(1, argv[1].length() - 2);
        return DumpCommand;
    }
    if (argc >= 1 && (argv[0] == "tick" || argv[0] == "t"))
    {
        if (argc == 1)
        {
            res = "1";
            return TickCommand;
        }
        if (argc == 2 && argv[1].length() > 4)
        {
            res = argv[1].substr(3, argv[1].length() - 4);
            return TickCommand;
        }
    }
    if (argc == 1 && argv[0] == "exit")
        return ExitCommand;
    if (argc == 1 && argv[0] == "help")
        return HelpCommand;

    return WrongCommand;
}

// Обновление
void GameController::update(Universe &world)
{
    // Ссчитывание команды с консоли
    std::string line;
    getline(std::cin, line);
    std::string res = "";
    CommandFromLine command = recognizeCommandFromLine(line, res);

    switch (command)
    {
    case (DumpCommand):
        world.upload(res.c_str());
        std::cout << "The universe was dumped in " << res << " file\n";
        break;

    case (TickCommand):
    {
        int n = atoi(res.c_str());
        if (n < 0)
        {
            std::cout << "The number of iterations must be greater than zero\n";
            break;
        }
        if (n == 0)
        {
            std::cout << "Wrong number of iterations\n";
            break;
        }
        world.nextStep(n);
        world.draw();
        break;
    }

    case (ExitCommand):
        inProcess_ = false;
        break;

    case (HelpCommand):
        std::cout << "Possible commands:\n";
        std::cout << "\tdump <filename> - Save the universe to the file with name filename\n";
        std::cout << "\ttick <n=1> (or t <n=1>) - Calculate n (default is 1) iterations and print the result\n";
        std::cout << "\texit - Finish the game\n";
        std::cout << "\thelp - Print the help about the commands\n";
        break;

    default:
        std::cout << "Wrong command. Type \"help\" to see possible commands\n";
        break;
    }
}

bool GameController::inProcess() const noexcept
{
    return inProcess_;
}
