#include "csv_writer.h"

#include <fstream>
#include <iomanip>

int CSVWriter::writeTextData(const TextData &td, std::string outFileName)
{
    std::ofstream outFile;
    outFile.open("data/output.csv");
    if (outFile.fail())
    {
        return 1;
    }

    const int wordsNum = td.getWordsNum();
    outFile << "Word\tFreq\tFreq(%)" << std::endl;
    for (auto i : (td.getWordsList()))
    {
        double percentage = static_cast<double>(i.first) / wordsNum * 100;
        outFile << i.second << "\t" << i.first << "\t" << std::fixed << std::setprecision(2) << percentage << '\n';
    }
    return 0;
}
