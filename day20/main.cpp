#include <vector>
#include <array>
#include <iostream>
#include <string>

using EnhanceAlgorithm = std::array<char, 512>;
using Position = std::pair<int, int>;

struct Pixels {
    std::vector<std::string> map;

    Position start;
    Position end;
};

class InputReader
{
public:
    static std::vector<std::string> read()
    {
        std::ios_base::sync_with_stdio(false);
        freopen(INPUT_FILE_PATH, "rt", stdin);

        std::vector<std::string> inputs;
        std::string input;

        while (!std::cin.eof())
        {
            getline(std::cin, input);
            if (input.empty())
            {
                continue;
            }

            inputs.emplace_back(input);
        }

        return inputs;
    }
};

class EnhanceAlgorithmMaker
{
public:
    static EnhanceAlgorithm make(const std::string& input)
    {
        EnhanceAlgorithm enhanceAlgorithm;

        for (int i = 0 ; i < input.size(); i++)
        {
            if (input[i] == '#')
            {
                enhanceAlgorithm[i] = '1';
            }
            else
            {
                enhanceAlgorithm[i] = '0';
            }
        }

        return enhanceAlgorithm;
    }
};

class PivotMover
{
public:
    PivotMover(const Position startPos, const Position endPos) :
        pivot(startPos),
        startPos(startPos),
        endPos(endPos) {}

    PivotMover()= default;

public:
    void move()
    {
        if(!canMove())
        {
            return;
        }

        if (shouldChangeLine())
        {
            pivot.first = startPos.first;
            pivot.second++;
        }
        else
        {
            pivot.first++;
        }
    }

    bool shouldChangeLine() const
    {
        return pivot.first == endPos.first;
    }

    bool canMove()
    {
        return pivot != Position{startPos.first, endPos.second + 1};
    }

    const Position &getPivot() const {
        return pivot;
    }

    const Position &getStartPos() const {
        return startPos;
    }

    const Position &getEndPos() const {
        return endPos;
    }

private:
    Position pivot;
    Position startPos;
    Position endPos;

};

class PixelsMaker
{
public:
    static Pixels make(const std::vector<std::string>& inputs)
    {
        Pixels pixels;

        size_t inputLineNumber = inputs.size() - 1;
        pixels.map.resize(inputLineNumber);
        pixels.end = Position {inputLineNumber - 1, inputLineNumber - 1};

        auto rowIndex = pixels.start.second;
        for (int i = 1 ; i <inputs.size(); i++)
        {
            for (char c : inputs[i])
            {
                if (c == '#')
                {
                    pixels.map[rowIndex].push_back('1');
                }
                else
                {
                    pixels.map[rowIndex].push_back('0');
                }
            }
            rowIndex++;
        }

        return pixels;
    }
};

class ImageEnhancer
{
public:
    explicit ImageEnhancer(EnhanceAlgorithm* enhanceAlgorithm)
        : enhanceAlgorithm(enhanceAlgorithm), pixels(nullptr), isFirstBitIsLit(enhanceAlgorithm->at(0) == '1'), enhanceCount(0) {}

    void setPixels(Pixels *pPixels) {
        pixels = pPixels;
    }

    void enhance()
    {
        pivotMover = PivotMover(
                {pixels->start.first - 1, pixels->start.second - 1},
                {pixels->end.first + 1, pixels->end.second + 1}
                );
        enhanceOutput.clear();

        while (pivotMover.canMove())
        {
            auto binary = extractBinaryFromPivot();
            auto algorithmIndex = std::stoi(binary, nullptr, 2);
            char algorithmValue = enhanceAlgorithm->at(algorithmIndex);
            enhanceOutput.push_back(algorithmValue);

            pivotMover.move();
        }

        updatePixels();
        enhanceCount++;
    }

private:
    std::string extractBinaryFromPivot()
    {
        std::string binary;
        const auto& pivot = pivotMover.getPivot();
        auto startRow = pivot.second - 1;
        auto startCols = pivot.first - 1;

        for (int y = 0 ; y < 3; y++)
        {
            for (int x = 0 ; x < 3; x++)
            {
                auto bit = getBitFromMap(startCols + x, startRow + y);
                binary.push_back(bit);
            }
        }

        return binary;
    }

    char getBitFromMap(int x, int y)
    {
        if (isPositionOutOfBound(x, y))
        {
            if (isFirstBitIsLit)
            {
                return (enhanceCount % 2 == 1) ? '1' : '0';
            }
            else
            {
                return '0';
            }
        }

        return pixels->map[y].at(x);
    }

    bool isPositionOutOfBound(int x, int y)
    {
        return x < pixels->start.first || y < pixels->start.second || x > pixels->end.first || y > pixels->end.second;
    }

    void updatePixels()
    {
        const auto& startPos = pivotMover.getStartPos();
        const auto& endPos = pivotMover.getEndPos();

        size_t width = endPos.first - startPos.first + 1;

        pixels->map.clear();

        int outputIndex = 0;
        for (int h = startPos.second; h <= endPos.second; h++)
        {
            pixels->map.emplace_back(enhanceOutput.begin() + outputIndex, enhanceOutput.begin() + outputIndex + width);
            outputIndex += width;
        }

        pixels->end = Position(pixels->end.first + 2, pixels->end.second + 2);
    }

private:
    const EnhanceAlgorithm *enhanceAlgorithm;
    Pixels *pixels;

    PivotMover pivotMover;
    std::string enhanceOutput;

    bool isFirstBitIsLit;
    size_t enhanceCount;
};

class LitCounter
{
public:
    static size_t count(const Pixels& pixels)
    {
        PivotMover pivotMover(pixels.start, pixels.end);

        size_t total = 0;

        while (pivotMover.canMove())
        {
            const auto& pivot = pivotMover.getPivot();
            if (pixels.map[pivot.second].at(pivot.first) == '1')
            {
                total++;
            }

            pivotMover.move();
        }

        return total;
    }
};

class PixelsPrinter
{
public:
    static void print(const Pixels& pixels)
    {
        PivotMover pivotMover(pixels.start, pixels.end);

        while (pivotMover.canMove())
        {
            const auto& pivot = pivotMover.getPivot();
            if (pixels.map[pivot.second].at(pivot.first) == '1')
            {
                printf("#");
            }
            else
            {
                printf(".");
            }

            if (pivot.first == pixels.end.first)
            {
                printf("\n");
            }

            pivotMover.move();
        }
    }
};

int main()
{
    const size_t MAX_REPEAT_NUM = 50;

    auto inputs = InputReader::read();
    EnhanceAlgorithm enhanceAlgorithm = EnhanceAlgorithmMaker::make(inputs[0]);
    Pixels pixels = PixelsMaker::make(inputs);

    ImageEnhancer imageEnhancer(&enhanceAlgorithm);
    imageEnhancer.setPixels(&pixels);

    for (int i = 0 ; i < MAX_REPEAT_NUM; i++)
    {
        imageEnhancer.enhance();

        if (i == 1)
        {
            std::cout << "20-1: " << LitCounter::count(pixels) << std::endl;
        }
    }

//    PixelsPrinter::print(pixels);
    std::cout << "20-2: " << LitCounter::count(pixels) << std::endl;
}