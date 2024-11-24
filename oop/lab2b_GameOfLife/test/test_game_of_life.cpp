
#include "initial_parameters.h"
#include "gtest/gtest.h"
#include <iostream>

TEST(game_of_life, initial_parameters)
{
    try
    {
        int argc = 6;
        char *argv[6] = {"game", "inp.txt", "-i", "10", "-o", "out.txt"};
        InitialParameters initParam(argc, argv);
        // std::cout << "gameMode: " << initParam.getGameMode() << "\n";
        // std::cout << "inputFilename: " << initParam.getInputFilename() << "\n";
        // std::cout << "outputFilename: " << initParam.getOutputFilename() << "\n";
        // std::cout << "iterations: " << initParam.getIterations() << "\n";
        ASSERT_EQ(initParam.getGameMode(), 2);
        ASSERT_EQ(initParam.getInputFilename(), "inp.txt");
        ASSERT_EQ(initParam.getOutputFilename(), "out.txt");
        ASSERT_EQ(initParam.getIterations(), 10);
    }
    catch (const InitialParameters::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}
