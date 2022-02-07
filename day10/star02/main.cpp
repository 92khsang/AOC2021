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

class CompletionFinder
{
public:
    std::vector<char> find(const Syntax& syntax)
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
                if (isDelimiterPairWithTop(delimiter))
                {
                    delimiterStack->pop();
                }
                else
                {
                    return std::vector<char>();
                }
            }

        }

        return makeCompletion();
    }

private:
    std::vector<char> makeCompletion()
    {
        std::vector<char> completions;

        while (!delimiterStack->empty())
        {
            const auto openChar = delimiterStack->top();
            delimiterStack->pop();

            const auto closeChar = delimierPairs.find(openChar)->second;
            completions.push_back(closeChar);
        }

        return completions;
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



};

class Grader
{
public:
    size_t grade(const std::vector<char>& completions)
    {
        size_t score = 0;
        for (const char& completion : completions)
        {
            score = (score * 5) +  delimiterScores.find(completion)->second;
        }

        return score;
    }

private:
    const std::unordered_map<char, size_t> delimiterScores = {
            {')', 1},
            {']', 2},
            {'}', 3},
            {'>', 4},
    };
};

int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    std::vector<size_t> scores;
    CompletionFinder finder;
    Grader grader;

    std::string input;
    while(!std::cin.eof())
    {
        getline(std::cin, input);
        auto syntax = InputParser::parse(input);

        const auto completion = finder.find(syntax);
        if (!completion.empty())
        {
            auto score = grader.grade(completion);
            scores.push_back(score);
        }
    }

    std::sort(scores.begin(), scores.end());

    std::cout << "Day 10-2: " << scores[(scores.size() / 2)] << " " << std::endl;

    return 0;
}


