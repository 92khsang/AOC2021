#include <bits/stdc++.h>
#include <utility>
#include <vector>
#include <array>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <stack>

struct Syntax
{
    std::vector<char> delimiters;
};

class InputParser
{
public:
    static Syntax parse(const std::string& input)
    {
        Syntax syntax;
        for (const auto& c : input)
        {
            syntax.delimiters.push_back(c);
        }

        return syntax;
    }
};

class SyntaxChecker
{
public:
    size_t getErrorScore(const Syntax& syntax)
    {
        delimiterStack.reset(new(std::stack<char>));

        for (const auto& delimiter : syntax.delimiters)
        {
            if (isOpenDelimiter(delimiter))
            {
                delimiterStack->push(delimiter);
                continue;
            }
            else
            {
                if (delimiterStack->empty())
                {
                    return getScore(delimiter);
                }
                else if (isDelimiterPairWithTop(delimiter))
                {
                    delimiterStack->pop();
                }
                else
                {
                    return getScore(delimiter);
                }

            }

        }

        return 0;
    }

private:
    size_t getScore(char delimiter) const
    {
        return delimiterScores.find(delimiter)->second;;
    }

    bool isOpenDelimiter(char delimiter) const
    {
        for (const auto& pair : delimierPairs)
        {
            if (delimiter == pair.first)
            {
                return true;
            }
        }

        return false;
    }

    bool isDelimiterPairWithTop(char delimiter) const
    {
        return (delimierPairs.find(delimiterStack->top())->second == delimiter);
    }

private:
    std::unique_ptr<std::stack<char>> delimiterStack;

    const std::unordered_map<char, char> delimierPairs = {
            {'(', ')'},
            {'[', ']'},
            {'{', '}'},
            {'<', '>'},
    };

    const std::unordered_map<char, size_t> delimiterScores = {
            {')', 3},
            {']', 57},
            {'}', 1197},
            {'>', 25137},
    };

};

int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    size_t totalScore = 0;
    SyntaxChecker checker;

    std::string input;
    while(!std::cin.eof())
    {
        getline(std::cin, input);
        auto syntax = InputParser::parse(input);

       totalScore += checker.getErrorScore(syntax);
    }


    std::cout << "Day 10-1: " << totalScore << " " << std::endl;

    return 0;
}


