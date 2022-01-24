#include <bits/stdc++.h>
#include <vector>
#include <unordered_map>

class Point
{
public:
    int32_t x;
    int32_t y;

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
        return std::hash<int32_t>()(point.x) ^ std::hash<int32_t>()(point.y);
    }
};

struct Direction
{
    int32_t x;
    int32_t y;
};

struct Line
{
    Point start;
    Point end;

    bool straight;
};

class LineMaker
{
public:
    static Line getLine(std::string input)
    {
        int32_t start_x = 0, start_y = 0, end_x = 0, end_y = 0;
        std::sscanf(input.c_str(), "%d,%d -> %d,%d", &start_x, &start_y, &end_x, &end_y);

        bool isStraight = (start_x == end_x || start_y == end_y);

        return Line{
                .start =  {
                        .x = start_x,
                        .y = start_y
                },
                .end = {
                        .x = end_x,
                        .y = end_y,
                },
                .straight = isStraight
        };
    }
};

class CrossCounter
{
public:
    void addLine(const Line& line)
    {
        Direction direction = getDirection(line);
        Point pivot = line.start;

        Point dest;
        dest.x = line.end.x + direction.x;
        dest.y = line.end.y + direction.y;

        while (dest != pivot)
        {
            counts[pivot]++;

            if (2 == counts[pivot])
            {
                dangerCount++;
            }

            pivot.x += direction.x;
            pivot.y += direction.y;
        }

    }

    size_t getDangerousPlaceCount() const
    {
        return dangerCount;
    }

private:
    Direction getDirection(const Line& line) const
    {
        Direction direction{};
        direction.x = getOneStep(line.end.x, line.start.x);
        direction.y = getOneStep(line.end.y, line.start.y);

        return direction;
    }

    int32_t getOneStep(int32_t endAxis, int32_t startAxis) const
    {
        int32_t gap = endAxis - startAxis;
        if (0 == gap)
        {
            return 0;
        }
        else if (gap < 0)
        {
            return -1;
        }
        else
        {
            return 1;
        }
    }

private:
    std::unordered_map<Point, size_t, point_hash> counts;
    size_t dangerCount = 0;


};


int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    std::string input;
    input.reserve(256);

    CrossCounter counter;

    while(!std::cin.eof())
    {
       getline(std::cin, input);
       Line newLine = LineMaker::getLine(input);

       if (!newLine.straight)
       {
           continue;
       }

       counter.addLine(newLine);
    }

    std::cout << "Day 05-1: " << counter.getDangerousPlaceCount() << " " << std::endl;

    return 0;
}


