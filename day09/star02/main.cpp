#include <bits/stdc++.h>
#include <utility>
#include <vector>
#include <array>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <queue>

class Point
{
public:
    Point() = default;
    Point(size_t inputX, size_t inputY) : x(inputX), y(inputY) {}

public:
    size_t x;
    size_t y;

    bool operator!=(const Point& other) const
    {
        return !(*this == other);
    }

    bool operator==(const Point& other) const
    {
        return (x == other.x && y == other.y);
    }

};

struct point_hash
{
    std::size_t operator() (const Point &point) const {
        return std::hash<size_t>()(point.x) ^ std::hash<size_t>()(point.y);
    }
};

class Basin
{
public:
    std::vector<Point> points;
};


class Map
{
public:
    void addDepth(const Point& point, uint16_t depth)
    {
        depths[point] = depth;
    }

    uint16_t getDepth(const Point& point) const
    {
        assert(depths.find(point) != depths.end());
        return depths.find(point)->second;
    }

    void clear()
    {
        depths.clear();
        width = 0;
        height = 0;
    }

public:
    size_t width = 0;
    size_t height = 0;

private:
    std::unordered_map<Point, uint16_t, point_hash> depths;
};

class MapMaker
{
public:
    void initMap()
    {
        map.clear();
    }

    void addLine(const std::string& line)
    {
        if (0 == map.width)
        {
            map.width = line.length();
        }

        for (size_t x = 0 ; x < line.length(); x++)
        {
            map.addDepth({x, map.height}, char2Uint16(line.at(x)));
        }

        map.height++;
    }

    Map getMap() const
    {
        return map;
    }

private:
    uint16_t char2Uint16(const char& c)
    {
        return c - 48;
    }

private:
    Map map;
};

class BasinsSearcher
{
public:
    std::vector<Basin> search(const Map& targetMap)
    {
        map = targetMap;

        std::vector<Basin> basins;

        Point pivot(0, 0);

        while(!isEndPoint(pivot))
        {
            if (9 == map.getDepth(pivot))
            {
                checkPoints[pivot] = true;
            }
            else if (!isAlreadyChecked(pivot))
            {
                auto basin = searchOneBasin(pivot);
                basins.push_back(basin);
            }

            movePivot(pivot);
        }

        return basins;
    }


private:
    Basin searchOneBasin(const Point& start)
    {
        Basin basin;
        std::queue<Point> basinQueue;
        basinQueue.push(start);

        while(!basinQueue.empty())
        {
            auto point = basinQueue.front();
            basinQueue.pop();
            if (isAlreadyChecked(point))
            {
                continue;
            }

            basin.points.push_back(point);
            checkPoints[point] = true;

            if (!isRightEdge(point) && !isRightBlocked(point))
            {
                basinQueue.emplace(point.x + 1, point.y);
            }

            if (!isBottomEdge(point) && !isBottomBlocked(point))
            {
                basinQueue.emplace(point.x, point.y + 1);
            }

            if (!isLeftEdge(point) && !isLeftBlocked(point))
            {
                basinQueue.emplace(point.x - 1, point.y);
            }

            if (!isTopEdge(point) && !isTopBlocked(point))
            {
                basinQueue.emplace(point.x, point.y - 1);
            }

        }

        return basin;
    }

    bool isRightEdge(const Point& point) const
    {
        return (point.x == (map.width - 1));
    }

    bool isBottomEdge(const Point& point) const
    {
        return (point.y == (map.height - 1));
    }

    bool isLeftEdge(const Point& point) const
    {
        return (point.x == 0);
    }

    bool isTopEdge(const Point& point) const
    {
        return (point.y == 0);
    }

    bool isRightBlocked(const Point& point) const
    {
        return (9 == map.getDepth({point.x + 1, point.y}));
    }

    bool isBottomBlocked(const Point& point) const
    {
        return (9 == map.getDepth({point.x, point.y + 1}));
    }

    bool isLeftBlocked(const Point& point) const
    {
        return (9 == map.getDepth({point.x - 1, point.y}));
    }

    bool isTopBlocked(const Point& point) const
    {
        return (9 == map.getDepth({point.x, point.y - 1}));
    }

    void movePivot(Point& point)
    {
        if (isRightEdge(point))
        {
            point.x = 0;
            point.y += 1;
        }
        else
        {
            point.x += 1;
        }
    }

    bool isAlreadyChecked(const Point& point) const
    {
        if (checkPoints.find(point) == checkPoints.end())
        {
            return false;
        }

        return true;
    }

    void checkRightPoint(const Point& point)
    {
        if (!isRightEdge(point))
        {
            checkPoints[{point.x + 1, point.y}] = true;
        }
    }

    void checkBottomPoint(const Point& point)
    {
        if (!isBottomEdge(point))
        {
            checkPoints[{point.x, point.y + 1}] = true;
        }
    }

    bool isEndPoint(const Point& point) const
    {
        return (isRightEdge(point) && isBottomEdge(point));
    }

private:
    Map map;
    std::unordered_map<Point, bool, point_hash> checkPoints;
};

class Answerer
{
public:
    static size_t answer(const std::vector<Basin>& basins)
    {
        std::vector<size_t> basinsSizes;
        basinsSizes.reserve(basins.size());

        for (const auto& basin : basins)
        {
            basinsSizes.push_back(basin.points.size());
        }

        std::sort(basinsSizes.begin(), basinsSizes.end());

        size_t multi = 1;
        size_t i = 3;
        for (auto it = basinsSizes.rbegin(); it != basinsSizes.rend(); it++)
        {
            if (i == 0)
            {
                break;
            }


            multi *= *it;

            i--;
        }

        return multi;
    }
};

int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    MapMaker maker;
    maker.initMap();

    std::string input;
    while(!std::cin.eof())
    {
        getline(std::cin, input);
        maker.addLine(input);
    }

    Map map = maker.getMap();

    BasinsSearcher searcher;
    auto basins = searcher.search(map);

    std::cout << "Day 09-2: " << Answerer::answer(basins) << " " << std::endl;

    return 0;
}


