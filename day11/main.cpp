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

struct Direction
{
    Direction(int8_t inputX, int8_t inputY) : x(inputX), y(inputY) {}

    int8_t x;
    int8_t y;
};

class Octopus
{
public:
    void setInitialEnergy(uint16_t initialEnergy)
    {
        energy = initialEnergy;
    }

    void flashed(size_t flashedTime)
    {
        if (isAlreadyFlashedAtThisTime(flashedTime))
        {
            return;
        }
        lastFlashingTime = flashedTime;

        energy++;

        if (10 == energy)
        {
            energy= 0;
            flashedCount++;
            flash(flashedTime);
        }
    }

    void addNeighbor(Octopus* neighbor)
    {
        neighbors.push_back(neighbor);
    }

    size_t getFlashedCount() const
    {
        return flashedCount;
    }

    uint16_t getEnergy() const
    {
        return energy;
    }

private:
    bool isAlreadyFlashedAtThisTime(int32_t flashedTime)
    {
        return (0 == energy && flashedTime == lastFlashingTime);
    }

    void flash(int32_t flashedTime)
    {
        for (const auto& neighbor : neighbors)
        {
            neighbor->flashed(flashedTime);
        }
    }

private:
    uint16_t energy = 0;
    size_t flashedCount = 0;
    size_t lastFlashingTime = 0;
    std::vector<Octopus *> neighbors;
};


class OctopusesMap
{
public:
    OctopusesMap(const OctopusesMap&) = delete;
    OctopusesMap(const OctopusesMap&&) = delete;

public:
    static OctopusesMap& getInstance()
    {
        static auto* instance = new OctopusesMap;
        return *instance;
    }

    void initEnergy(const Point& point, uint16_t energy)
    {
        octopuses[point.y][point.x].setInitialEnergy(energy);
    }

    Octopus& getOctopus(const Point& point)
    {
        return octopuses[point.y][point.x];
    }

    void show()
    {
        printf("\n");
        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                printf("%u", octopuses[y][x].getEnergy());
            }
            printf("\n");
        }
    }

public:
    static const size_t WIDTH = 10;
    static const size_t HEIGHT = 10;

private:
    OctopusesMap()
    {
        for (int y = 0; y < HEIGHT; y++)
        {
            for (int x = 0; x < WIDTH; x++)
            {
                auto& octopus = octopuses[y][x];

                for (const auto& direction : directions)
                {
                    int32_t neighborX = x + direction.x;
                    int32_t neighborY = y + direction.y;

                    if ((neighborX >= 0 && neighborX < WIDTH)  && (neighborY >= 0 && neighborY < HEIGHT))
                    {
                        octopus.addNeighbor(&octopuses[neighborY][neighborX]);
                    }
                }
            }
        }
    }

    std::array<std::array<Octopus, 10>, 10> octopuses;
    const std::array<Direction, 8> directions = {
            Direction(-1, -1),
            Direction(-1, 0),
            Direction(-1, 1),
            Direction(0, -1),
            Direction(0, 1),
            Direction(1, -1),
            Direction(1, 0),
            Direction(1, 1),
    };
};

class OctopusesMapInitializer
{
public:
    void addLine(const std::string& line)
    {
        for (size_t x = 0 ; x < line.length(); x++)
        {
            OctopusesMap::getInstance().initEnergy({x, height}, char2Uint16(line.at(x)));
        }

        height++;
    }

private:
    static uint16_t char2Uint16(const char& c)
    {
        return c - 48;
    }

    uint16_t height = 0;
};

class Submarine
{
public:
    void flash(size_t flashIndex)
    {
        auto& octopusesMap = OctopusesMap::getInstance();

        for (size_t y = 0; y < OctopusesMap::HEIGHT; y++)
        {
            for (size_t x = 0; x < OctopusesMap::WIDTH; x++)
            {
                auto& octopus = octopusesMap.getOctopus({x, y});
                octopus.flashed(flashIndex);
            }
        }
//        octopusesMap.show();
    }
};

class Counter
{
public:
    size_t count()
    {
        size_t sum = 0;

        auto& octopusesMap = OctopusesMap::getInstance();
        for (size_t y = 0; y < OctopusesMap::HEIGHT; y++)
        {
            for (size_t x = 0; x < OctopusesMap::WIDTH; x++)
            {
                auto& octopus = octopusesMap.getOctopus({x, y});
                sum += octopus.getFlashedCount();
            }
        }

        return sum;
    }

};

class OctopusesFlashChecker
{
public:
    static bool isAllOctopusesFlashed()
    {
        auto& octopusesMap = OctopusesMap::getInstance();
        for (size_t y = 0; y < OctopusesMap::HEIGHT; y++)
        {
            for (size_t x = 0; x < OctopusesMap::WIDTH; x++)
            {
                auto& octopus = octopusesMap.getOctopus({x, y});
                if (0 != octopus.getEnergy())
                {
                    return false;
                }
            }
        }

        return true;
    }
};

int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    OctopusesMapInitializer initializer;

    std::string input;
    while(!std::cin.eof())
    {
        getline(std::cin, input);
        initializer.addLine(input);
    }

    size_t NUM_OF_FLASH = 100;
    Submarine submarine;
    Counter counter;

    size_t flashedCount = 0;
    size_t timeOfAllOctopusesFlashed = 0;

    for (size_t i = 1 ; ; i++)
    {
        submarine.flash(i);

        if (i == NUM_OF_FLASH)
        {
            flashedCount = counter.count();
        }

        if (OctopusesFlashChecker::isAllOctopusesFlashed())
        {
            timeOfAllOctopusesFlashed = i;
            break;
        }

    }

    std::cout << "Day 11-1: " << flashedCount << " " << std::endl;
    std::cout << "Day 11-2: " << timeOfAllOctopusesFlashed << " " << std::endl;

    return 0;
}


