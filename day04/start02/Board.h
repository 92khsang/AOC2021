//
// Created by hayes on 1/6/22.
//

#ifndef ADVENTOFCODE2021_BOARD_H
#define ADVENTOFCODE2021_BOARD_H

#include <iostream>

#include <array>
#include <unordered_map>
#include <map>
#include <cassert>

class Direction
{
public:
    enum
    {
        UP,
        LEFT,
        DOWN,
        RIGHT
    };

    static const size_t MAX_DIRECTION_COUNT = 4;

    static int32_t getOppositeDirection(int32_t direction)
    {
        if (direction < 2)
        {
            return direction + 2;
        }
        else
        {
            return direction - 2;
        }
    }
};

struct Movement
{
    Movement(int32_t xMove, int32_t yMove) : x(xMove), y(yMove){}

    int32_t x = 0;
    int32_t y = 0;
};

struct Location
{
public:
    Location() = default;
    Location(const Location& location) = default;
    Location(int32_t xLocation, int32_t yLocation) : x(xLocation), y(yLocation){}

    int32_t x = 0;
    int32_t y = 0;
};


class Point
{
public:
    Location location;
    uint32_t value = 0;

    bool marked = false;

    void updateFarthestPoint(Point& newPoint, int32_t direction)
    {
        farthestBingoPoint[direction] = &newPoint;
    }

    Point* findFarthestMarkedPoint(int32_t direction)
    {
        const auto& farthestPoint = farthestBingoPoint[direction];

        if (nullptr == farthestPoint)
        {
            assert(!marked);
            return farthestPoint;
        }
        else if (this == farthestPoint)
        {
            return this;
        }
        else
        {
            const auto& newFarthestLocation = farthestPoint->findFarthestMarkedPoint(direction);
            farthestPoint->farthestBingoPoint[direction] = newFarthestLocation;
            return newFarthestLocation;
        }
    }

private:
    std::array<Point *, Direction::MAX_DIRECTION_COUNT> farthestBingoPoint{};
};

class Board
{
public:
    static const std::array<Movement, Direction::MAX_DIRECTION_COUNT> movement;

    enum
    {
        BOARD_SIZE = 5
    };

    static bool isOutOfBound(int32_t x, int32_t y) ;

    void addNewPoint(int32_t x, int32_t y, uint32_t value);
    Point& getPoint(int32_t x, int32_t y);
    Point& getPoint(uint32_t value);
    void Bingo();

    bool DoesPointExist(uint32_t value) const;
    bool HasBingo() const;

private:
    Point points[BOARD_SIZE + 1][BOARD_SIZE + 1];
    std::unordered_map<uint32_t, Location> map;

    bool bingo = false;
};


#endif //ADVENTOFCODE2021_BOARD_H
