#ifndef TEXT_DATA_H
#define TEXT_DATA_H

#include <list>
#include <string>

class TextData
{
private:
    std::list<std::pair<int, std::string>> wordsList;

public:
    struct FileFail
    {
        std::string fileName;
    };
    TextData(std::string inFileName);
    const std::list<std::pair<int, std::string>> &getWordsList() const noexcept;
    int getWordsNum() const noexcept;
};

#endif
