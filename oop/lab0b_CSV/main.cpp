#include <fstream>
#include <iostream>

#include "csv_writer.h"
#include "text_data.h"

int main(int argv, char **args)
{
    if (argv < 3)
    {
        std::cerr << "There is not enough arguments\n";
        return 0;
    }

    TextData *td;
    try
    {
        td = new TextData(args[1]); // "data/input.txt"
    }
    catch (std::bad_alloc ba)
    {
        std::cerr << "Memory not allocated\n";
        std::cerr << ba.what() << "\n";
        return 0;
    }
    catch (const TextData::FileFail &ex)
    {
        std::cerr << "Somthing wrong with " << ex.fileName << '\n';
        return 0;
    }

    CSVWriter writer;
    if (writer.writeTextData(*td, args[2])) // "data/output.csv"
    {
        std::cerr << "Somthing wrong with " << args[2] << '\n';
        // std::cerr << "Somthing wrong with out file\n";
        delete td;
        return 0;
    }

    delete td;

    return 0;
}
