#include <bits/stdc++.h>
#include <utility>
#include <vector>
#include <queue>
#include <unordered_map>
#include <cassert>
#include <iostream>

class Dot
{
public:
    Dot() = default;
    Dot(size_t inputX, size_t inputY) : x(inputX), y(inputY) {}

public:
    size_t x;
    size_t y;

    bool operator!=(const Dot& other) const
    {
        return !(*this == other);
    }

    bool operator==(const Dot& other) const
    {
        return (x == other.x && y == other.y);
    }
};

struct dot_hash
{
    std::size_t operator() (const Dot &dot) const {
        return std::hash<size_t>()(dot.x) ^ std::hash<size_t>()(dot.y);
    }
};

using DotsOnAxis = std::unordered_map<uint32_t, std::unordered_set<Dot, dot_hash>>;

struct Dots
{
    DotsOnAxis xPoints;
    DotsOnAxis yPoints;
};

class FoldLine
{
public:
    FoldLine(char foldAxis, uint32_t foldLocation) : axis(foldAxis), location(foldLocation){}

    const char axis;
    const uint32_t location;
};

class DotMaker
{
public:
    static Dots make()
    {
        Dots dots;

        std::string input;
        while(!std::cin.eof())
        {
            getline(std::cin, input);
            if (input.empty())
            {
                break;
            }

            auto newDot = pares(input);
            dots.xPoints[newDot.x].insert(newDot);
            dots.yPoints[newDot.y].insert(newDot);
        }

        return dots;
    }

private:
    static Dot pares(const std::string& input)
    {
        const char delimiter = ',';

        auto foundedIndex = input.find(delimiter);

        auto xStr = input.substr(0, foundedIndex);
        auto yStr = input.substr(foundedIndex + 1, input.size());

        auto x = std::stoul(xStr);
        auto y = std::stoul(yStr);

        return {x, y};
    }
};

class FoldLineMaker
{

public:
    static std::queue<FoldLine> make()
    {

        std::queue<FoldLine> lines;
        std::string input;
        while(!std::cin.eof())
        {
            getline(std::cin, input);

            lines.push(pares(input));
        }

        return lines;
    }

private:
    static FoldLine pares(const std::string& input)
    {
        const char delimiter = '=';

        auto foundedIndex = input.find(delimiter);

        auto foldAxisStr = input.substr(foundedIndex - 1, foundedIndex);
        auto foldLocStr = input.substr(foundedIndex + 1, input.size());

        auto foldAxis = foldAxisStr.at(0);
        auto foldLoc = std::stoul(foldLocStr);

        return {foldAxis, static_cast<uint32_t>(foldLoc)};
    }
};

class Folder
{
public:
    void static fold(Dots& dots, const FoldLine line)
    {
        if (line.axis == 'x')
        {
            foldAxis(line.location, dots.xPoints, dots.yPoints);
        }
        else
        {
            foldAxis(line.location, dots.yPoints, dots.xPoints);
        }
    }

private:
    void static foldAxis(uint32_t foldLoc, DotsOnAxis& foldedAxisPoints, DotsOnAxis& otherAxisPoints)
    {
        for (auto it = foldedAxisPoints.begin(); it != foldedAxisPoints.end();)
        {
            auto dotLoc = it->first;
            if (dotLoc < foldLoc)
            {
                it++;
                continue;
            }

            auto foldedLoc = (foldLoc * 2) - dotLoc;

            for (auto dot : it->second)
            {
                auto dotOtherAxisLoc = (dotLoc == dot.x) ? dot.y : dot.x;
                otherAxisPoints[dotOtherAxisLoc].erase(dot);
                if (dotLoc == dot.x)
                {
                    dot.x = foldedLoc;
                }
                else
                {
                    dot.y = foldedLoc;
                }

                otherAxisPoints[dotOtherAxisLoc].insert(dot);
                foldedAxisPoints[foldedLoc].insert(dot);
            }

            auto deleteIt = it;
            it++;
            foldedAxisPoints.erase(deleteIt);
        }
    }

};

class DotCounter
{
public:
    static size_t count(const Dots& dots)
    {
        size_t count = 0;
        for (const auto & xPoint : dots.xPoints)
        {
            count += xPoint.second.size();
        }

        return count;
    }
};

class DotPrinter
{
public:
    static void print(const Dots& dots, uint32_t width, uint32_t height)
    {
        for (uint32_t y = 0; y < height; y++)
        {
            for (uint32_t x = 0; x < width; x++)
            {
                if (dots.yPoints.find(y) != dots.yPoints.end())
                {
                    if (dots.yPoints.find(y)->second.find({x, y}) != dots.yPoints.find(y)->second.end())
                    {
                        printf("#");
                        continue;
                    }
                }

                printf(".");
            }
            printf("\n");
        }
    }
};

int main(){
    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    auto dots = DotMaker::make();
    auto foldLines = FoldLineMaker::make();

    Folder::fold(dots, foldLines.front());
    foldLines.pop();

    std::cout << "Day 13-1: " << DotCounter::count(dots) << " " << std::endl;

    uint32_t lastFoldedX = 0, lastFoldedY = 0;
    while (!foldLines.empty())
    {
        auto foldLine = foldLines.front();
        foldLines.pop();

        Folder::fold(dots, foldLine);

        if (foldLine.axis == 'x')
        {
            lastFoldedX = foldLine.location;
        }
        else
        {
            lastFoldedY = foldLine.location;
        }
    }

    std::cout << "Day 13-2: " << " " << std::endl;
    DotPrinter::print(dots, lastFoldedX, lastFoldedY);

    return 0;
}


