#include <bits/stdc++.h>
#include <utility>
#include <vector>
#include <array>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <stack>

struct InitialInput
{
    std::string input;

    char leftEnd;
    char rightEnd;
};

struct PairMap
{
    using Parent = std::string;
    using Children = std::array<std::string, 2>;

    std::unordered_map<Parent, Children> items;
};

class InputParser
{
public:
    static InitialInput makeInitialInput()
    {
        InitialInput initialInput;


        getline(std::cin, initialInput.input);

        initialInput.leftEnd = initialInput.input.at(0);
        initialInput.rightEnd = initialInput.input.back();

        clearOneLIne();

        return initialInput;
    }

    static PairMap makePairMap()
    {
        PairMap map;

        std::string input;
        while(!std::cin.eof())
        {
            getline(std::cin, input);
            auto pairAndElement = parsePairAndElementFrom(input);

            const auto& pair = pairAndElement.first;
            map.items[pair] = makeChildrenFrom(pairAndElement);
        }

        return map;
    }

private:
    static void clearOneLIne()
    {
        std::string input;
        getline(std::cin, input);
    }

    static std::pair<std::string, char> parsePairAndElementFrom(const std::string& input)
    {
        char pair[3];
        char element;
        std::sscanf(input.c_str(), "%s -> %c", pair, &element);

        return {std::string(pair), element};
    }

    static PairMap::Children makeChildrenFrom(const std::pair<std::string, char>& input)
    {
        PairMap::Children children;

        children[0].push_back(input.first[0]);
        children[0].push_back(input.second);

        children[1].push_back(input.second);
        children[1].push_back(input.first[1]);

        return children;
    }

};

class PairCounter
{
public:
    static std::unordered_map<std::string, size_t> count(const std::string& firstInput)
    {
        std::unordered_map<std::string, size_t> pairCount;

        for (int i = 0 ; i < firstInput.size() - 1; i++)
        {
            std::string pair;
            pair.reserve(2);

            pair.push_back(firstInput[i]);
            pair.push_back(firstInput[i + 1]);

            pairCount[pair] = pairCount[pair] + 1;
        }

        return pairCount;
    }

    static std::unordered_map<std::string, size_t> count(const std::unordered_map<std::string, size_t>& current, const PairMap& pairMap)
    {
        std::unordered_map<std::string, size_t> next;

        for (const auto& pivot : current)
        {
            const auto& pair = pivot.first;
            const auto& count = pivot.second;

            for (const auto& child : pairMap.items.find(pair)->second)
            {
                next[child] += count;
            }
        }

        return next;
    }
};

class CharCounter
{
public:
    static std::unordered_map<char, size_t> count(const InitialInput& initialInput, const std::unordered_map<std::string, size_t>& pairCount)
    {
        std::unordered_map<char, size_t> charCounts;

        charCounts[initialInput.leftEnd] = 1;
        charCounts[initialInput.rightEnd] = 1;

        for (const auto& pivot : pairCount)
        {
            const auto& pair = pivot.first;
            const auto& count = pivot.second;

            charCounts[pair[0]] += count;
            charCounts[pair[1]] += count;
        }

        for (auto& charCount : charCounts)
        {
            charCount.second = (charCount.second / 2);
        }

        return charCounts;
    }
};

class Answerer
{
public:
    static size_t answer(std::unordered_map<char, size_t> charCounts)
    {
        size_t mostCommonElementCount = charCounts.begin()->second;
        size_t leastCommonElementCount = charCounts.begin()->second;

        for (const auto& charCount : charCounts)
        {
            if (mostCommonElementCount < charCount.second)
            {
                mostCommonElementCount = charCount.second;
            }

            if (leastCommonElementCount > charCount.second)
            {
                leastCommonElementCount = charCount.second;
            }
        }

        return (mostCommonElementCount - leastCommonElementCount);
    }
};

int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    auto initialInput = InputParser::makeInitialInput();
    auto pairMap = InputParser::makePairMap();


    auto pivot = PairCounter::count(initialInput.input);

    const size_t STAR_ONE_MAX_LOOP_COUNT = 10;
    size_t loopCount = 0;
    for (loopCount = 0 ; loopCount < STAR_ONE_MAX_LOOP_COUNT; loopCount++)
    {
        pivot = PairCounter::count(pivot, pairMap);
    }

    auto charCounts = CharCounter::count(initialInput, pivot);

    std::cout << "Day 14-1: " << Answerer::answer(charCounts) << " " << std::endl;

    const size_t STAR_TWO_MAX_LOOP_COUNT = 40;
    for (;loopCount < STAR_TWO_MAX_LOOP_COUNT; loopCount++)
    {
        pivot = PairCounter::count(pivot, pairMap);
    }

    charCounts = CharCounter::count(initialInput, pivot);
    std::cout << "Day 14-2: " << Answerer::answer(charCounts) << " " << std::endl;


    return 0;
}


