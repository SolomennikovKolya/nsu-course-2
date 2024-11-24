
#include "initial_parameters.h"
#include <cstdlib>
#include <iostream>
#include <vector>

InitialParameters::InitialParameters(int argc, char **argv0)
{
    std::vector<std::string> argv(argc);
    for (int i = 0; i < argc; ++i)
        argv[i] = argv0[i];

    switch (argc)
    {
    case 1:
        gameMode_ = OnlineWithRandomField;
        break;

    case 2:
        if (argv[1] == "--help")
        {
            gameMode_ = Tutorial;
            std::cout << "You can start the game in 3 different game mods:\n";
            std::cout << "\t1. OnlineWithRandomField\n";
            std::cout << "\t\tTo start this game mode type \".\\gameOfLife.exe\"\n";
            std::cout << "\t2. OnlineWithFieldFromFile\n";
            std::cout << "\t\tTo start this game mode type \".\\gameOfLife.exe inp.txt\"\n";
            std::cout << "\t3. Offline\n";
            std::cout << "\t\tTo start this game mode type \".\\gameOfLife.exe inp.txt -i x -o out.txt\"\n";
            std::cout << "\t\tAlso you can type            \".\\gameOfLife.exe inp.txt --iterations=x --output=out.txt\"\n";
            std::cout << "\t\tWhere x is number of iterations\n";
            break;
        }
        else
        {
            gameMode_ = OnlineWithFieldFromFile;
            inputFilename_ = argv[1];
            break;
        }

    default: // argc >= 3
        gameMode_ = Offline;
        inputFilename_ = argv[1];

        int argNum = 2;
        if (argv[argNum] == "-i")
        {
            argNum++;
            if (argNum >= argc)
                throw InitialParameters::exception("You need to enter number of iterations after \"-i\". For example \"10\".");
            iterations_ = atoi(argv0[argNum]);
            if (iterations_ == 0)
                throw InitialParameters::exception("You need to enter valid number of iterations after \"-i\". It needs to be int value greater than zero.");
        }
        else if (argv[argNum].length() > 13 && argv[argNum].substr(0, 13) == "--iterations=")
        {
            iterations_ = atoi(argv[argNum].substr(13, argv[argNum].length() - 13).c_str());
            if (iterations_ == 0)
                throw InitialParameters::exception("You need to enter valid number of iterations after \"--iterations=\". It needs to be int value greater than zero.");
        }
        else
        {
            throw InitialParameters::exception("You need to enter number of iterations after input filename. For example \"-i 10\" or \"--iterations=10\".");
        }

        argNum++;
        if (argNum >= argc)
            throw InitialParameters::exception("You need to enter output filename. For example \"-o output_file.txt\" or \"--output=output_file.txt\".");
        if (argv[argNum] == "-o")
        {
            argNum++;
            if (argNum >= argc)
                throw InitialParameters::exception("You need to enter output filename after \"-o\". For example \"output_file.txt\"");
            outputFilename_ = argv[argNum];
        }
        else if (argv[argNum].length() > 9 && argv[argNum].substr(0, 9) == "--output=")
        {
            outputFilename_ = argv[argNum].substr(9, argv[argNum].length() - 9);
        }
        else
        {
            throw InitialParameters::exception("You need to enter valid output filename. For example \"-o output_file.txt\" or \"--output=output_file.txt\".");
        }

        if (argc > argNum + 1)
            throw InitialParameters::exception("You need to enter more arguments.");

        break;
    }
}

InitialParameters::InitialParameters(const InitialParameters &other)
{
    gameMode_ = other.gameMode_;
    inputFilename_ = other.inputFilename_;
    outputFilename_ = other.outputFilename_;
    iterations_ = other.iterations_;
}

static std::string getStringFromGameMode(GameMode gm)
{
    switch (gm)
    {
    case (0):
        return "OnlineWithRandomField";
    case (1):
        return "OnlineWithFieldFromFile";
    case (2):
        return "Offline";
    default:
        return "Tutorial";
    }
}

GameMode InitialParameters::getGameMode() const noexcept
{
    return gameMode_;
}

const char *InitialParameters::getInputFilename() const
{
    if (gameMode_ == OnlineWithRandomField)
        throw InitialParameters::exception("You cannot get inputFilename in the OnlineWithRandomField game mode.");
    else if (gameMode_ == Tutorial)
        throw InitialParameters::exception("You cannot get inputFilename in the Tutorial game mode.");
    else
        return inputFilename_.c_str();
}

const char *InitialParameters::getOutputFilename() const
{
    if (gameMode_ == OnlineWithRandomField)
        throw InitialParameters::exception("You cannot get outputFilename in the OnlineWithRandomField game mode.");
    else if (gameMode_ == OnlineWithFieldFromFile)
        throw InitialParameters::exception("You cannot get outputFilename in the OnlineWithFieldFromFile game mode.");
    else if (gameMode_ == Tutorial)
        throw InitialParameters::exception("You cannot get outputFilename in the Tutorial game mode.");
    else
        return outputFilename_.c_str();
}

int InitialParameters::getIterations() const
{
    if (gameMode_ == OnlineWithRandomField)
        throw InitialParameters::exception("You cannot get Iterations in the OnlineWithRandomField game mode.");
    else if (gameMode_ == OnlineWithFieldFromFile)
        throw InitialParameters::exception("You cannot get Iterations in the OnlineWithFieldFromFile game mode.");
    else if (gameMode_ == Tutorial)
        throw InitialParameters::exception("You cannot get Iterations in the Tutorial game mode.");
    else
        return iterations_;
}
