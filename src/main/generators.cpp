#include "generators.h"

using namespace Utility::Generators;


void Utility::Generators::WordVectors::addWords(const std::vector<std::string> &input, std::vector<std::string> &output)
{
    for (const std::string & word : input)
        output.push_back(word);
}



std::string Utility::Generators::generateText(int countMin, int countMax, const std::vector<std::string> &initWords)
{
    std::string result;
    std::string bufferWordString;
    const int initWordsSize = initWords.size();

    if (countMin == countMax)
        countMax++;

    const int wordCount = (std::rand() % (countMax - countMin) + countMin);

    bool isSentenceStart = true;

    for (int i = 0; i < wordCount; i++)
    {
        bufferWordString = initWords[ std::rand() % initWordsSize ];

        if (isSentenceStart)
        {
            bufferWordString[0] = std::toupper(bufferWordString[0]);
            isSentenceStart = false;
        }

        if (std::rand() % 2)
        {
            switch (std::rand() % 4)
            {
            case 0:
                bufferWordString += '.';
                break;

            case 1:
                bufferWordString += '?';
                break;

            case 2:
                bufferWordString += '!';
                break;

            case 3:
                bufferWordString += "...";
                break;
            }
            isSentenceStart = true;
        }

        if (i != wordCount - 1)
            bufferWordString += " ";
        else
        {
            if (bufferWordString[bufferWordString.size() - 1] != '.')
                bufferWordString += '.';
        }

        result += bufferWordString.c_str();
    }
    return result;
}

std::string Utility::Generators::generateString(int sizeMin, int sizeMax, bool hasSpaces)
{
    std::string result;
    char symbol;
    int currentSymbolType;

    if (sizeMin == sizeMax)
        sizeMax++;

    for (int i = 0; i < (std::rand() % (sizeMax - sizeMin) + sizeMin); i++)
    {
        if (hasSpaces)
            currentSymbolType = std::rand() % 4;
        else
            currentSymbolType = std::rand() % 3;

        switch (currentSymbolType)
        {
        case 0:
            symbol = std::rand() % 26 + 'a'; // Only small
            break;

        case 1:
            symbol = std::rand() % 26 + 'A'; // Only small
            break;

        case 2:
            symbol = std::rand() % 9 + '0'; // Only number
            break;

        case 3:
            symbol = ' '; // Spaces
            break;
        }
        result += symbol;
    }
    return result;
}

std::string Utility::Generators::generateAboba(int countMin, int countMax)
{
    std::string result;
    std::string abobaString = "aboba";

    if (countMin == countMax)
        countMax++;

    for (int i = 0; i < (std::rand() % (countMax - countMin) + countMin); i++)
    {
        for (auto & s : abobaString)
        {
            if (std::rand() % 2)
                s = std::tolower(s);
            else
                s = std::toupper(s);
        }

        result += " ";
        result += abobaString.c_str();
    }
    return result;
}

std::vector<std::string> Utility::Generators::generateDepends(std::vector<std::string> &dependTargets, const std::string &target, int targetCount, const int randomInterval)
{
    std::vector<std::string> dependFrom;
    std::string dependBuffer;

    if (!targetCount)
        targetCount++;

    for (int j = 1; j < (std::rand() % randomInterval); j++)
    {
        dependBuffer = dependTargets[std::rand() % targetCount];
        if (dependBuffer != target)
        {
            dependFrom.push_back(dependBuffer);
        }
    }

    return dependFrom;
}

std::string Utility::Generators::generatePath(bool withSpaces)
{
    std::string result = "/";
    for (int i = 0; i < std::rand() % 15; i++)
        result += generateString(2, 20, withSpaces) + "/";
    return result;
}




#ifdef QT_CORE_LIB
QString Utility::Generators::generateTextQ(int countMin, int countMax, const std::vector<std::string> &initWords)
{
    QString result;
    std::string bufferWordString;
    const int initWordsSize = initWords.size();

    if (countMin == countMax)
        countMax++;

    const int wordCount = (std::rand() % (countMax - countMin) + countMin);

    bool isSentenceStart = true;

    for (int i = 0; i < wordCount; i++)
    {
        bufferWordString = initWords[ std::rand() % initWordsSize ];

        if (isSentenceStart)
        {
            bufferWordString[0] = std::toupper(bufferWordString[0]);
            isSentenceStart = false;
        }

        if (std::rand() % 2)
        {
            switch (std::rand() % 4)
            {
            case 0:
                bufferWordString += '.';
                break;

            case 1:
                bufferWordString += '?';
                break;

            case 2:
                bufferWordString += '!';
                break;

            case 3:
                bufferWordString += "...";
                break;
            }
            isSentenceStart = true;
        }

        if (i != wordCount - 1)
            bufferWordString += " ";
        else
        {
            if (bufferWordString[bufferWordString.size() - 1] != '.')
                bufferWordString += '.';
        }

        result += bufferWordString.c_str();
    }
    return result;
}

QString Utility::Generators::generateStringQ(int sizeMin, int sizeMax, bool hasSpaces)
{
    QString result;
    char symbol;
    int currentSymbolType;

    if (sizeMin == sizeMax)
        sizeMax++;

    for (int i = 0; i < (std::rand() % (sizeMax - sizeMin) + sizeMin); i++)
    {
        if (hasSpaces)
            currentSymbolType = std::rand() % 4;
        else
            currentSymbolType = std::rand() % 3;

        switch (currentSymbolType)
        {
        case 0:
            symbol = std::rand() % 26 + 'a'; // Only small
            break;

        case 1:
            symbol = std::rand() % 26 + 'A'; // Only small
            break;

        case 2:
            symbol = std::rand() % 9 + '0'; // Only number
            break;

        case 3:
            symbol = ' '; // Spaces
            break;
        }
        result += symbol;
    }
    return result;
}

QString Utility::Generators::generateAbobaQ(int countMin, int countMax)
{
    QString result;
    std::string abobaString = "aboba";

    if (countMin == countMax)
        countMax++;

    for (int i = 0; i < (std::rand() % (countMax - countMin) + countMin); i++)
    {
        for (auto & s : abobaString)
        {
            if (std::rand() % 2)
                s = std::tolower(s);
            else
                s = std::toupper(s);
        }

        result += " ";
        result += abobaString.c_str();
    }
    return result;
}

QVector<QString> Utility::Generators::generateDependsQ(QVector<QString> &dependTargets, const QString &target, int targetCount, const int randomInterval)
{
    QVector<QString> dependFrom;

    QString dependBuffer;

    if (!targetCount)
        targetCount++;

    for (int j = 1; j < (std::rand() % randomInterval); j++)
    {
        dependBuffer = dependTargets[std::rand() % targetCount];
        if (dependBuffer != target)
            dependFrom.push_back(dependBuffer);
    }

    return dependFrom;
}

QString Utility::Generators::generatePathQ(bool withSpaces)
{
    QString result = "/";
    for (int i = 0; i < std::rand() % 15; i++)
    {
        result += generateStringQ(2, 20, withSpaces) + "/";
    }
    return result;
}
#endif // QT_CORE_LIB
