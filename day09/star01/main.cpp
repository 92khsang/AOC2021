#include <bits/stdc++.h>
#include <utility>
#include <vector>
#include <array>
#include <unordered_map>
#include <cassert>
#include <iostream>

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

class LowestPointSearcher
{
public:
    std::vector<Point> search(const Map& targetMap)
    {
        checkPoints.clear();
        map = targetMap;

        std::vector<Point> lowestPoints;

        Point pivot(0, 0);
        while(!isEndPoint(pivot))
        {

            bool isPivotLowerThanRight = false;
            bool isPivotLowerThanBottom = false;

            if (isLowerThanRight(pivot))
            {
                checkRightPoint(pivot);
                isPivotLowerThanRight = true;
            }

            if (isLowerThanBottom(pivot))
            {
                checkBottomPoint(pivot);
                isPivotLowerThanBottom = true;
            }

            if (!isAlreadyChecked(pivot) && (isPivotLowerThanRight && isPivotLowerThanBottom)) {
                lowestPoints.push_back(pivot);
            }

            checkPoints[pivot] = true;

            movePivot(pivot);
        }

        return lowestPoints;
    }


private:
    bool isRightEdge(const Point& point) const
    {
        return (point.x == (map.width - 1));
    }

    bool isBottomEdge(const Point& point) const
    {
        return (point.y == (map.height - 1));
    }

    bool isLowerThanRight(const Point& point) const
    {
        if (isRightEdge(point))
        {
            return true;
        }
        else
        {
            auto targetDepth = map.getDepth(point);
            auto compareDepth = map.getDepth({point.x + 1, point.y});

            if (targetDepth < compareDepth)
            {
                return true;
            }
        }

        return false;
    }

    bool isLowerThanBottom(const Point& point) const
    {
        if (isBottomEdge(point))
        {
            return true;
        }
        else
        {
            auto targetDepth = map.getDepth(point);
            auto compareDepth = map.getDepth({point.x, point.y + 1});

            if (targetDepth < compareDepth)
            {
                return true;
            }
        }

        return false;
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

class RiskCalculator
{
public:
    static size_t calculate(const Map& map, const std::vector<Point>& lowestPoints)
    {
        size_t sum = 0;

        for (const auto& lowestPoint : lowestPoints)
        {
            sum += map.getDepth(lowestPoint);
        }

        return sum + lowestPoints.size();
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

    LowestPointSearcher searcher;
    const auto lowestPoints = searcher.search(map);

    std::cout << "Day 09-1: " << RiskCalculator::calculate(map, lowestPoints) << " " << std::endl;

    return 0;
}


