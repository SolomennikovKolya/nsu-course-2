#include "text_data.h"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>

static bool checkLetter(char c)
{
    return ('0' <= c && c <= '9' || 'a' <= c && c <= 'z' || 'A' <= c && c <= 'Z');
}

static void fillDataFromFileToMap(std::map<std::string, int> *wordsMap, std::ifstream *inFile)
{
    std::string s;
    std::string tmp = "";

    while (!(inFile->eof()))
    {
        std::getline(*inFile, s);
        for (auto c : s)
        {
            if (checkLetter(c))
                tmp += c;
            else if (tmp != "")
            {
                (*wordsMap)[tmp]++;
                tmp.clear();
            }
        }
    }
    if (tmp != "")
    {
        (*wordsMap)[tmp]++;
        tmp.clear();
    }
}

bool comp(const std::pair<int, std::string> &a, const std::pair<int, std::string> &b)
{
    return (a.first > b.first);
}

TextData::TextData(std::string inFileName)
{
    std::ifstream inFile;
    inFile.open(inFileName);
    if (inFile.fail())
    {
        throw FileFail(inFileName);
    }

    std::map<std::string, int> wordsMap;
    fillDataFromFileToMap(&wordsMap, &inFile);
    inFile.close();

    for (auto &elem : wordsMap)
    {
        wordsList.insert(wordsList.begin(), std::make_pair(elem.second, elem.first));
    }

    wordsList.sort(comp);
    // wordsList.reverse();
}

const std::list<std::pair<int, std::string>> &TextData::getWordsList() const noexcept
{
    return wordsList;
}

int TextData::getWordsNum() const noexcept
{
    return static_cast<int>(wordsList.size());
}
