#include <bits/stdc++.h>
#include <utility>
#include <vector>
#include <array>
#include <unordered_map>
#include <cassert>
#include <iostream>

struct SevenSegment
{
    char top;
    char leftTop;
    char rightTop;
    char mid;
    char leftBot;
    char rightBot;
    char bot;
};

struct DigitalNumber
{
    std::array<std::vector<char>, 10> numbers{};
    SevenSegment segment{};
};

class StrConverter
{
public:
    static std::vector<char> convertString2CharVec(const std::string& src)
    {
        std::vector<char> dest(src.size());

        for (int i = 0 ; i < src.size(); i++)
        {
            dest[i] = src.at(i);
        }

        std::sort(dest.begin(), dest.end());

        return dest;
    }

};


class DigitalNumberMaker
{
public:
    DigitalNumber make(const std::vector<std::string>& newSignals)
    {
        unknownSignals.clear();
        knownSignals.clear();
        clearSegment();

        for (const auto& newSignal : newSignals)
        {
            addSignalAsChar(newSignal);
        }

        Infer();

        DigitalNumber digitalNumber;
        digitalNumber.segment = segment;
        for (const auto& signal : knownSignals)
        {
            digitalNumber.numbers[signal.first] = signal.second;
        }

        return digitalNumber;
    }


private:
    void clearSegment()
    {
        segment.top = 0;
        segment.mid = 0;
        segment.bot = 0;
        segment.leftTop = 0;
        segment.leftBot = 0;
        segment.rightTop = 0;
        segment.rightBot = 0;
    }

    void addSignalAsChar(const std::string& signal)
    {
        auto vector = StrConverter::convertString2CharVec(signal);
        unknownSignals.push_back(vector);
    }

    void Infer()
    {

        findClearNumber(1, 2);
        findClearNumber(4, 4);
        findClearNumber(7, 3);
        findClearNumber(8, 7);

        InferTopChar();

        findNumberNine();
        InferBotChar();
        InferLeftBotChar();

        findNumberZero();
        InferLeftTopChar();

        findNumberThree();
        findNumberSix();
        findNumberFive();
        findNumberTwo();


        InferMidChar();
        InferRightTopChar();
        InferRightBotChar();

    }

    void findClearNumber(const uint32_t targetNumber, const size_t numberOfChars)
    {
        assert(knownSignals.find(targetNumber) == knownSignals.end());

        for (auto unknownSignal = unknownSignals.begin(); unknownSignal != unknownSignals.end(); unknownSignal++)
        {
            if (numberOfChars == unknownSignal->size())
            {
                knownSignals[targetNumber] = *unknownSignal;
                unknownSignals.erase(unknownSignal);
                break;
            }
        }
    }

    void findNumberNine()
    {
        assert(knownSignals.find(4) != knownSignals.end());
        assert(segment.top != 0);

        const auto& number = knownSignals[4];
        std::vector<char> tempDigit(number.begin(), number.end());
        addCharToVector(segment.top, tempDigit);

        findNumberWhereOneCharIsDiff(tempDigit, 9);
    }

    void findNumberZero()
    {
        assert(knownSignals.find(7) != knownSignals.end());
        assert(segment.bot != 0);
        assert(segment.leftBot != 0);

        const auto& number = knownSignals[7];
        std::vector<char> tempDigit(number.begin(), number.end());
        addCharToVector(segment.bot, tempDigit);
        addCharToVector(segment.leftBot, tempDigit);

        findNumberWhereOneCharIsDiff(tempDigit, 0);
    }

    void findNumberThree()
    {
        assert(knownSignals.find(9) != knownSignals.end());
        assert(segment.bot != 0);
        assert(segment.leftBot != 0);

        const auto& number = knownSignals[9];
        std::vector<char> tempDigit(number.begin(), number.end());
        deleteCharToVector(segment.leftTop, tempDigit);

        findNumber(tempDigit, 3);
    }


    void findNumberSix()
    {
        assert(unknownSignals.size() == 3);
        assert(knownSignals.find(8) != knownSignals.end());

        findNumberWhereOneCharIsDiff(knownSignals[8], 6);
    }

    void findNumberFive()
    {
        assert(unknownSignals.size() == 2);
        assert(knownSignals.find(6) != knownSignals.end());

        findNumberWhereOneCharIsDiff(knownSignals[6], 5);
    }


    void findNumberTwo()
    {
        assert(unknownSignals.size() == 1);

        knownSignals[2] = unknownSignals[0];
        unknownSignals.erase(unknownSignals.begin());
    }


    void InferTopChar()
    {
        assert(knownSignals.find(1) != knownSignals.end());
        assert(knownSignals.find(7) != knownSignals.end());

        segment.top = findOneOtherChar(knownSignals[1], knownSignals[7]);
    }

    void InferBotChar()
    {
        assert(knownSignals.find(9) != knownSignals.end());
        assert(knownSignals.find(4) != knownSignals.end());
        assert(segment.top != 0);

        const auto& numberFour = knownSignals[4];
        std::vector<char> tempDigit(numberFour.begin(), numberFour.end());
        addCharToVector(segment.top, tempDigit);

        segment.bot = findOneOtherChar(tempDigit, knownSignals[9]);
    }

    void InferLeftBotChar()
    {
        assert(knownSignals.find(9) != knownSignals.end());
        assert(knownSignals.find(8) != knownSignals.end());

        segment.leftBot = findOneOtherChar(knownSignals[8], knownSignals[9]);
    }

    void InferLeftTopChar()
    {
        assert(knownSignals.find(0) != knownSignals.end());
        assert(knownSignals.find(7) != knownSignals.end());
        assert(segment.bot != 0);
        assert(segment.leftBot != 0);

        const auto& numberFour = knownSignals[7];
        std::vector<char> tempDigit(numberFour.begin(), numberFour.end());
        addCharToVector(segment.bot, tempDigit);
        addCharToVector(segment.leftBot, tempDigit);

        segment.leftTop = findOneOtherChar(tempDigit, knownSignals[0]);
    }

    void InferMidChar()
    {
        assert(knownSignals.find(8) != knownSignals.end());
        assert(knownSignals.find(0) != knownSignals.end());

        segment.mid = findOneOtherChar(knownSignals[8], knownSignals[0]);
    }

    void InferRightTopChar()
    {
        assert(knownSignals.find(9) != knownSignals.end());
        assert(knownSignals.find(5) != knownSignals.end());

        segment.rightTop = findOneOtherChar(knownSignals[9], knownSignals[5]);
    }

    void InferRightBotChar()
    {
        assert(knownSignals.find(1) != knownSignals.end());
        assert(segment.rightTop != 0);

        const auto& one = knownSignals.find(1);
        for (const auto& oneChar : one->second)
        {
            if (oneChar != segment.rightTop)
            {
                segment.rightBot = oneChar;
                return;
            }
        }

    }

    void findNumber(const std::vector<char>& compare, const uint32_t targetNumber)
    {
        for (auto unknownSignal = unknownSignals.begin(); unknownSignal != unknownSignals.end(); unknownSignal++)
        {
            if (std::equal(compare.begin(), compare.end(), unknownSignal->begin()))
            {
                knownSignals[targetNumber] = *unknownSignal;
                unknownSignals.erase(unknownSignal);
                break;
            }
        }
    }


    void findNumberWhereOneCharIsDiff(const std::vector<char>& compare, const uint32_t targetNumber)
    {

        for (auto unknownSignal = unknownSignals.begin(); unknownSignal != unknownSignals.end(); unknownSignal++)
        {
            const std::vector<char>& smallVector = (compare.size() < unknownSignal->size()) ? compare : *unknownSignal;
            const std::vector<char>& largeVector = (compare.size() < unknownSignal->size()) ? *unknownSignal : compare;

            if (1 != (largeVector.size() - smallVector.size()))
            {
                continue;
            }

            size_t diffCount = 0;
            uint32_t i = 0;
            for (i = 0 ; i < smallVector.size(); i++)
            {
                if (diffCount > 1)
                {
                    break;
                }

                if (smallVector[i] != largeVector[i + diffCount])
                {
                    diffCount++;
                    i--;
                }
            }

            if (diffCount <= 1)
            {
                knownSignals[targetNumber] = *unknownSignal;
                unknownSignals.erase(unknownSignal);
                break;
            }
        }
    }



    static void addCharToVector(char newChar, std::vector<char>& target)
    {
        for (auto iter = target.begin(); iter != target.end(); iter++)
        {
            if (*iter == newChar)
            {
                return;
            }
            else if (newChar < *iter)
            {
                target.insert(iter, newChar);
                return;
            }
        }
        target.push_back(newChar);
    }


    static void deleteCharToVector(char oldChar, std::vector<char>& target)
    {
        for (auto iter = target.begin(); iter != target.end(); iter++)
        {
            if (*iter == oldChar)
            {
                target.erase(iter);
                return;
            }

        }
    }

    static char findOneOtherChar(std::vector<char>& left, std::vector<char>& right)
    {
        const std::vector<char>& smallVector = (left.size() < right.size()) ? left : right;
        const std::vector<char>& largeVector = (left.size() < right.size()) ? right : left;

        uint32_t i = 0;
        for (i = 0 ; i < smallVector.size(); i++)
        {
            if (smallVector[i] != largeVector[i])
            {
                break;
            }
        }

        if (i != smallVector.size())
        {
            return largeVector[i];
        }
        else
        {
            return largeVector.back();
        }
    }


private:
    std::vector<std::vector<char>> unknownSignals;
    std::unordered_map<uint16_t, std::vector<char>> knownSignals;
    SevenSegment segment{};
};

class DigitalNumberConverter
{
public:
    explicit DigitalNumberConverter(DigitalNumber digitalNumber)
        : number(std::move(digitalNumber))
    {
    }

    uint32_t convert(const std::string& input) const
    {
        for (uint32_t i = 0 ; i < number.numbers.size(); i++)
        {
            const auto& convertedInput = StrConverter::convertString2CharVec(input);
            if (isSameVector(i, convertedInput))
            {
                return i;
            }
        }
        assert(false);
    }

private:
    bool isSameVector(uint32_t index, const std::vector<char>& target) const
    {
        const auto& compare = number.numbers[index];

        if (target.size() != compare.size())
        {
            return false;
        }

        for (size_t i = 0 ; i < target.size(); i++)
        {
            if (compare[i] != target[i])
            {
                return false;
            }
        }

        return true;
    }

    DigitalNumber number;
};

class DigitalNumberCounter
{
public:
    DigitalNumberCounter(const DigitalNumberConverter& digitalNumberConverter, std::vector<uint32_t> numberToCheck, std::vector<std::string> targets)
        : converter(digitalNumberConverter), checklist(std::move(numberToCheck)), targetStrings(std::move(targets)) {}

    size_t getTotalCount() const
    {
        size_t count = 0;

        for (const auto& target : targetStrings)
        {
            uint32_t convertedTarget = converter.convert(target);
            if (isNumberIncludedInCheckList(convertedTarget))
            {
                count++;
            }
        }

        return count;
    }

private:
    bool isNumberIncludedInCheckList(uint32_t number) const
    {
        for (const auto& pivot : checklist)
        {
            if (number == pivot)
            {
                return true;
            }
        }

        return false;
    }

    const DigitalNumberConverter& converter;
    std::vector<uint32_t> checklist;
    std::vector<std::string> targetStrings;

};

class DigitalNumberCompleter
{
public:
    DigitalNumberCompleter(const DigitalNumberConverter& digitalNumberConverter, std::vector<std::string> targets)
            : converter(digitalNumberConverter), targetStrings(std::move(targets)) {}

    uint32_t complete() const
    {
        uint32_t digit = 0;

        for (const auto& target : targetStrings)
        {
            uint32_t convertedTarget = converter.convert(target);
            digit = digit * 10 + convertedTarget;
        }

        return digit;
    }

private:
    const DigitalNumberConverter& converter;
    std::vector<std::string> targetStrings;

};

void tokenize(std::string const &str, const char delim,
              std::vector<std::string> &out)
{
    std::stringstream ss(str);

    std::string s;
    while (std::getline(ss, s, delim)) {
        out.push_back(s);
    }
}


int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    std::vector<uint32_t> numberToCheck{1, 4, 7, 8};

    DigitalNumberMaker maker;

    size_t totalCheckNumberCount = 0;
    size_t sumOfOutputNumber = 0;

    std::string input;
    while(!std::cin.eof())
    {
        getline(std::cin, input);

        const auto& division = input.find('|');
        std::string patternInput = input.substr(0, division - 1);
        std::string outputInput = input.substr(division + 1, input.size());

        if (' ' == outputInput.at(0))
        {
            outputInput.erase(outputInput.begin());
        }

        std::vector<std::string> patterns;
        std::vector<std::string> output;

        tokenize(patternInput, ' ', patterns);
        tokenize(outputInput, ' ', output);

        DigitalNumber digitalNumber = maker.make(patterns);
        DigitalNumberConverter converter(digitalNumber);
        DigitalNumberCounter counter(converter, numberToCheck, output);
        DigitalNumberCompleter completer(converter, output);

        totalCheckNumberCount += counter.getTotalCount();
        sumOfOutputNumber += completer.complete();
    }

    std::cout << "Day 08-1: " << totalCheckNumberCount << " " << std::endl;
    std::cout << "Day 08-2: " << sumOfOutputNumber << " " << std::endl;

    return 0;
}


