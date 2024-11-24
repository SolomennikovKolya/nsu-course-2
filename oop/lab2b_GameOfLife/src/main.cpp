
#include "game_controller.h"
#include <iostream>

using namespace std;

int main(int argc, char **argv)
{
    try
    {
        InitialParameters initParam(argc, argv);
        if (initParam.getGameMode() == Tutorial)
            return 0;

        Universe world;      // Объявление вселенной
        GameController game; // Объявление игрового контроллера

        if (initParam.getGameMode() == Offline)
        {
            game.processOfflineMode(initParam, world);
            return 0;
        }

        // Далее только OnlineWithRandomField или OnlineWithFieldFromFile режим
        game.start(initParam, world);
        game.draw(world);

        while (game.inProcess())
        {
            game.update(world);
        }
    }
    catch (const InitialParameters::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
    catch (const Field::exception &e)
    {
        std::cerr << e.what() << '\n';
    }

    return 0;
}
