#include <bits/stdc++.h>
#include <utility>
#include <vector>
#include <array>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <stack>
#include <queue>

#define INF 10000

struct Map
{
    std::vector<std::vector<int16_t>> costs;

    int16_t width = 0;
    int16_t height = 0;
};

class MapMaker
{
public:
    Map make()
    {
        map = Map();

        int16_t lineCount = 0;

        std::string input;
        while(!std::cin.eof())
        {
            getline(std::cin, input);

            if (lineCount == 0)
            {
                map.width = input.size();
            }

            std::vector<int16_t> newLine;
            newLine.reserve(map.width);

            fillNewLine(newLine, input);

            map.costs.push_back(newLine);
            lineCount++;
        }

        map.height = lineCount;

        return map;
    }

private:
    void fillNewLine(std::vector<int16_t>& newLine, const std::string& input)
    {
        for (const char& numberChar : input)
        {
            newLine.push_back(numberChar - 48);
        }
    }

    Map map;
};

class LargeMapMaker
{
public:
    enum
    {
        MULTIPLY_NUMBER = 5,
        MAX_RISK = INF
    };

    Map make(const Map& originalMap)
    {
        largeMap = originalMap;

        extendWidths();
        extendHeights();

        largeMap.width = originalMap.width * MULTIPLY_NUMBER;
        largeMap.height = originalMap.height * MULTIPLY_NUMBER;

        return largeMap;
    }

private:
    void extendWidths()
    {
        for (int16_t y = 0 ; y < largeMap.height; y++)
        {
            auto& width = largeMap.costs[y];
            extendWidth(width);
        }
    }

    void extendWidth(std::vector<int16_t>& width)
    {
        auto previousWidthSize = width.size();
        width.resize(previousWidthSize * MULTIPLY_NUMBER);
        for (int16_t x = 0 ; x < previousWidthSize; x++)
        {
            for (int16_t multiplyStep = 1; multiplyStep < MULTIPLY_NUMBER; multiplyStep++)
            {
                int16_t previousRiskX = (multiplyStep - 1) * previousWidthSize + x;
                int16_t previousRisk = width[previousRiskX];
                int16_t nextRisk = getNextRiskLevel(previousRisk);
                int16_t nextRiskX = multiplyStep * previousWidthSize + x;

                width[nextRiskX] = nextRisk;
            }
        }
    }

    void extendHeights()
    {
        largeMap.costs.reserve(largeMap.costs.size() * MULTIPLY_NUMBER);

        for (int16_t multiplyStep = 1; multiplyStep < MULTIPLY_NUMBER; multiplyStep++)
        {
            for (int16_t y = 0 ; y < largeMap.height; y++)
            {
                int16_t previousRiskY = (multiplyStep - 1) * largeMap.height + y;
                auto& previousRiskLine = largeMap.costs[previousRiskY];
                auto nextRiskLine = getNextRiskLine(previousRiskLine);
                largeMap.costs.push_back(nextRiskLine);
            }
        }
    }

    std::vector<int16_t> getNextRiskLine(const std::vector<int16_t>& previousRiskLine)
    {
        std::vector<int16_t> nextRiskLine = previousRiskLine;

        for (short & x : nextRiskLine)
        {
            x = getNextRiskLevel(x);
        }

        return nextRiskLine;
    }

    int16_t getNextRiskLevel(const int16_t previousRisk)
    {
        int16_t next = previousRisk + 1;
        if (next > 9)
        {
            next = 1;
        }
        return next;
    }

    Map largeMap;
};


class MinCostRiskMapMaker
{
public:
    Map make(const Map& originalMap)
    {
        using cost = int16_t;
        using point = std::pair<int16_t, int16_t>;

        init(originalMap);
        std::priority_queue<std::pair<cost, point>> costQ;
        costQ.push({0, {0, 0}});

        while(!costQ.empty())
        {
            auto cost = -costQ.top().first;
            auto point = costQ.top().second;
            costQ.pop();

            auto currentX = point.first;
            auto currentY = point.second;

            for (const auto& direction : DIRECTIONS)
            {
                int16_t nextX = currentX + direction.first;
                int16_t nextY = currentY + direction.second;

                if (isVisitable(nextX, nextY))
                {
                    if (minDistMap.costs[nextY][nextX] > cost + originalMap.costs[nextY][nextX])
                    {
                        minDistMap.costs[nextY][nextX] = cost + originalMap.costs[nextY][nextX];
                        costQ.push({-minDistMap.costs[nextY][nextX], {nextX, nextY}});
                    }
                }
            }
        }

        return minDistMap;
    }

private:
    void init(const Map& originalMap)
    {
        minDistMap.width = originalMap.width;
        minDistMap.height = originalMap.height;

        minDistMap.costs.resize(minDistMap.height);

        for (int16_t y = 0; y < minDistMap.height ; y++)
        {
            minDistMap.costs[y].resize(minDistMap.width);
            for (int16_t x = 0 ; x < minDistMap.width; x++)
            {
                minDistMap.costs[y][x] = INF;
            }
        }
    }

    bool isVisitable(int32_t x, int32_t y)
    {
        return (x >= 0 && x < minDistMap.width) && (y >= 0 && y < minDistMap.height);
    }

private:
    Map minDistMap;

    const std::vector<std::pair<int16_t, int16_t>> DIRECTIONS = {
        {-1, 0},
        {1, 0},
        {0, 1},
        {0, -1},
    };

};

int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    MapMaker mapMaker;
    MinCostRiskMapMaker riskMapMaker;

    auto map = mapMaker.make();
    auto riskMap = riskMapMaker.make(map);

    std::cout << "Day 15-1: " <<  riskMap.costs[riskMap.height - 1][riskMap.width - 1] << " " << std::endl;

    LargeMapMaker largeMapMaker;
    auto largeMap = largeMapMaker.make(map);
    auto largeRiskMap = riskMapMaker.make(largeMap);

    std::cout << "Day 15-2: " <<  largeRiskMap.costs[largeRiskMap.height - 1][largeRiskMap.width - 1] << " " << std::endl;

    return 0;
}


