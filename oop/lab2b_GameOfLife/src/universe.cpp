
#include "universe.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

Universe::Universe() noexcept {}

Universe::~Universe() noexcept {}

void Universe::fillTransitionRuleByDefault() noexcept
{
    bool birth0[9] = {0, 0, 0, 1, 0, 0, 0, 0, 0};
    bool survival0[9] = {0, 0, 1, 1, 0, 0, 0, 0, 0};
    for (int i = 0; i < 9; ++i)
    {
        rule_.birth[i] = birth0[i];
        rule_.survival[i] = survival0[i];
    }
}

// Ссчитывает правило перехода из строки вида B3/S23 в котейнеры bool newBirthRule[9], bool newSurvivalRule[9]
// Возврящает false если всё ссчиталось правильно, и true, если строка заполнена неверно (в этом случае пишет предупреждения в std::cerr)
static bool inputUniversalRule(std::string subline, bool newBirthRule[9], bool newSurvivalRule[9])
{
    int pos = 0; // Expect subline[pos] = 'B'
    if (subline.length() < pos + 1)
        return true;
    if (subline[pos] != 'B')
    {
        std::cout << "Universal rule needs to start with \"B\"\n";
        return true;
    }

    pos++;
    if (subline.length() < pos + 1)
        return true;
    while (subline[pos] != '/')
    {
        int i = subline[pos] - '0';
        if (i < 0 || i > 8)
        {
            std::cerr << "Wrong birth rule. Digits needs to be in interval [0, 8]\n";
            return 1;
        }
        newBirthRule[i] = 1;
        pos++;
        if (subline.length() < pos + 1)
            return true;
    }

    pos++; // Expect subline[pos] = 'S'
    if (subline.length() < pos + 1)
        return true;
    if (subline[pos] != 'S')
    {
        std::cerr << "It needs to be \"S\" after \"/\"\n";
        return true;
    }

    pos++;
    while (pos < subline.length())
    {
        int i = subline[pos] - '0';
        if (i < 0 || i > 8)
        {
            std::cerr << "Wrong survival rule. Digits needs to be in interval [0, 8]\n";
            return true;
        }
        newSurvivalRule[i] = 1;
        pos++;
    }
    return false;
}

void Universe::inputData(std::ifstream &inp, const char *inputFilename) noexcept
{
    std::string line;
    if (!std::getline(inp, line))
        return;
    if (line != "#Life 1.06")
    {
        std::cout << "Expected file format (\"#Life 1.06\") in the first line of " << inputFilename << "\n";
        return;
    }

    if (!std::getline(inp, line))
        return;
    if (line.length() > 3 && line.substr(0, 3) == "#N ")
        name_ = line.substr(3, line.length() - 3);
    else
    {
        std::cerr << "Expected name of universe (for example \"#N My universe\") in the second line of " << inputFilename << "\n";
        return;
    }

    if (!std::getline(inp, line))
        return;
    if (line.length() > 3 && line.substr(0, 3) == "#R ")
    {
        bool newBirthRule[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        bool newSurvivalRule[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};
        if (inputUniversalRule(line.substr(3, line.length() - 3), newBirthRule, newSurvivalRule))
        {
            std::cerr << "Incorrect format of transition rule (expected for example \"#R B3/S23\")\n";
            return;
        }
        for (int i = 0; i < 9; ++i)
        {
            rule_.birth[i] = newBirthRule[i];
            rule_.survival[i] = newSurvivalRule[i];
        }
    }
    else
    {
        std::cerr << "Expected rule of universe (for example \"#R B3/S23\") in the third line of " << inputFilename << "\n";
        return;
    }
}

// Ссчитывает координаты из строки
// Возвращает false, если всё ссчиталось правильно, и true, если строка заполнена неверно
static bool coordianatesFromLine(std::string line, int &x, int &y)
{
    std::istringstream iss(line);
    if (!(iss >> x >> y))
        return true;
    return false;
}

void Universe::inputCoordinates(std::ifstream &inp) noexcept
{
    std::string line;
    std::vector<std::pair<int, int>> coords;

    while (std::getline(inp, line))
    {
        int x, y;
        if (coordianatesFromLine(line, x, y))
        {
            std::cerr << "Expected line fith coordinates (for example \"5 10\")\n";
            break;
        }
        coords.push_back(std::make_pair(x, y));
    }

    if (coords.size() == 0)
    {
        field_.init(MIN_FIELD_W, MIN_FIELD_H);
        for (int y = 0; y < field_.getHeight(); ++y)
        {
            for (int x = 0; x < field_.getWidth(); ++x)
                field_[y][x] = 0;
        }
        return;
    }

    int minX = coords[0].first, maxX = coords[0].first;
    int minY = coords[0].second, maxY = coords[0].second;
    for (int i = 0; i < coords.size(); ++i)
    {
        const int x = coords[i].first;
        const int y = coords[i].second;
        if (x < minX)
            minX = x;
        if (x > maxX)
            maxX = x;
        if (y < minY)
            minY = y;
        if (y > maxY)
            maxY = y;
    }

    int w = maxX - minX + 1;
    int h = maxY - minY + 1;
    if (w < MIN_FIELD_W)
        w = MIN_FIELD_W;
    if (h < MIN_FIELD_H)
        h = MIN_FIELD_H;

    field_.init(w, h);
    for (int y = 0; y < field_.getHeight(); ++y)
    {
        for (int x = 0; x < field_.getWidth(); ++x)
            field_[y][x] = 0;
    }
    for (int i = 0; i < coords.size(); ++i)
    {
        bool &item = field_.item(coords[i].first, coords[i].second);
        if (item)
            std::cerr << "It is better not to repeat the coordinates in the input file\n";
        else
            item = true;
    }
}

void Universe::load(const char *inputFilename)
{
    fillTransitionRuleByDefault();
    name_ = "Default Universe Name";
    iterations_ = 0;

    std::ifstream inp(inputFilename);
    if (inp.fail() || !inp.is_open())
    {
        std::cout << "Error when trying to load universe from " << inputFilename << " file\n";
        return;
    }

    inputData(inp, inputFilename);
    inputCoordinates(inp);

    nextField_.init(field_.getWidth(), field_.getHeight());
}

void Universe::init(const InitialParameters &initParam)
{
    switch (initParam.getGameMode())
    {
    case (OnlineWithRandomField):
        field_.createRandom();
        nextField_.init(DEFAULT_FIELD_W, DEFAULT_FIELD_H);
        fillTransitionRuleByDefault();
        name_ = "Random Universe";
        iterations_ = 0;
        break;

    case (OnlineWithFieldFromFile):
        load(initParam.getInputFilename());
        break;
    }
}

static int intLen(int x)
{
    if (x < 0)
        x *= -1;
    if (x == 0)
        return 1;
    int ans = 0;
    while (x > 0)
    {
        ans++;
        x /= 10;
    }
    return ans;
}

std::string Universe::getRuleStr() const noexcept
{
    std::string ruleStr = "B";
    for (int i = 0; i < 8; ++i)
    {
        if (rule_.birth[i])
            ruleStr += '0' + i;
    }
    ruleStr += "/S";
    for (int i = 0; i < 8; ++i)
    {
        if (rule_.survival[i])
            ruleStr += '0' + i;
    }
    return ruleStr;
}

void Universe::draw() const noexcept
{
    for (int i = 0; i < INDENT; ++i)
        std::cout << "\n";

    for (int i = 0; i < field_.getWidth() - int(name_.length()) / 2; ++i)
        std::cout << " ";
    std::cout << "\"" << name_ << "\"\n";

    std::string ruleStr = getRuleStr();
    for (int i = 0; i < field_.getWidth() + 1 - int(ruleStr.length()) / 2; ++i)
        std::cout << " ";
    std::cout << ruleStr << "\n";

    for (int i = 0; i < field_.getWidth() + 1 - (12 + intLen(iterations_)) / 2; ++i)
        std::cout << " ";
    std::cout << "Iterations: " << iterations_ << "\n";

    field_.draw();
    std::cout << "\n";
}

void Universe::nextStep(unsigned int n) noexcept
{
    if (n == 0)
        return;

    // Относительные координаты соседних клеток
    int dx[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    int dy[8] = {-1, -1, -1, 0, 0, 1, 1, 1};

    for (int y = 0; y < field_.getHeight(); ++y)
    {
        for (int x = 0; x < field_.getWidth(); ++x)
        {
            int livingNeighbors = 0;
            for (int i = 0; i < 8; ++i)
            {
                if (field_.item(x + dx[i], y + dy[i]))
                    livingNeighbors++;
            }
            if (field_[y][x])
                nextField_[y][x] = rule_.survival[livingNeighbors];
            else
                nextField_[y][x] = rule_.birth[livingNeighbors];
        }
    }
    field_.copy(nextField_);
    iterations_++;
    nextStep(n - 1);
}

void Universe::upload(const char *outputFilename) const noexcept
{
    std::ofstream out(outputFilename);
    if (out.fail() || !out.is_open())
    {
        std::cout << "Error when trying to dump universe to " << outputFilename << " file\n";
        return;
    }
    out << "#Life 1.06\n";
    out << "#N " << name_ << "\n";
    out << "#R " << getRuleStr() << "\n";
    for (int y = 0; y < field_.getHeight(); ++y)
    {
        for (int x = 0; x < field_.getWidth(); ++x)
        {
            if (field_[y][x])
                out << x << " " << y << "\n";
        }
    }
}
