#include <bits/stdc++.h>
#include <utility>
#include <vector>
#include <array>
#include <unordered_map>
#include <cassert>
#include <iostream>

class Cave
{
public:
    explicit Cave(uint16_t caveType, std::string caveName) : TYPE(caveType), CAVE_NAME(std::move(caveName)){}
    Cave(const Cave& cave) = delete;
    Cave(const Cave&& cave) = delete;

public:
    void visited()
    {
        visitedCount++;
    }

    void released()
    {
        visitedCount--;
    }

    size_t getVisitedCount() const
    {
        return visitedCount;
    }

    void connectWith(Cave* connectedCave)
    {
        connectedCaves.push_back(connectedCave);
    }

    std::vector<Cave *> getConnectedCaves() const
    {
        return connectedCaves;
    }

public:
    enum
    {
        START_CAVE,
        BIG_CAVE,
        SMALL_CAVE,
        END_CAVE,
    };

    const uint16_t TYPE;
    const std::string CAVE_NAME;

private:
    size_t visitedCount = 0;
    std::vector<Cave *> connectedCaves;
};

class InputParser
{
public:
    static std::array<std::string, 2> parse(const std::string& input)
    {
        const char delimiter = '-';

        auto foundedIndex = input.find(delimiter);
        return {input.substr(0, foundedIndex), input.substr(foundedIndex + 1, input.size())};
    }
};

class CaveMaker
{
public:
    static std::unique_ptr<Cave> make(const std::string& caveName)
    {
        auto caveType = judgeCaveType(caveName);
        auto newCave = new Cave(caveType, caveName);
        return std::unique_ptr<Cave>(newCave);
    }

private:
    static uint16_t judgeCaveType(const std::string& caveName)
    {
        const char firstChar = caveName.at(0);
        if (isupper(firstChar))
        {
            return Cave::BIG_CAVE;
        }
        else
        {
            if (caveName == "start")
            {
                return Cave::START_CAVE;
            }
            else if (caveName == "end")
            {
                return Cave::END_CAVE;
            }
        }

        return Cave::SMALL_CAVE;
    }
};

class CaveConnector
{
public:
    static void connect(std::pair<Cave *, Cave *> cave)
    {
        assert(cave.first != nullptr);
        assert(cave.second != nullptr);

        connectLeftToRight(cave.first, cave.second);
        connectLeftToRight(cave.second, cave.first);
    }

private:
    static void connectLeftToRight(Cave *left, Cave *right)
    {
        if (Cave::END_CAVE != left->TYPE && Cave::START_CAVE != right->TYPE)
        {
            left->connectWith(right);
        }
    }
};

class CavePathPrinter
{
public:
    static void printAll(const std::vector<std::vector<Cave *>>& paths)
    {
        for (const auto& path : paths)
        {
            print(path);
        }
    }

    static void print(const std::vector<Cave *>& path)
    {
        std::stringstream stream;
        for (const auto& cave : path)
        {
            stream.write(cave->CAVE_NAME.c_str(), cave->CAVE_NAME.length());
            if (Cave::END_CAVE != cave->TYPE)
            {
                stream.write(",", 1);
            }
        }
        std::cout << stream.str() << std::endl;
    }
};



class CaveTraveler
{
public:
    explicit CaveTraveler(Cave* startCave) : start(startCave) {}
    CaveTraveler(CaveTraveler&) = delete;
    CaveTraveler(CaveTraveler&&) = delete;


public:
    std::vector<std::vector<Cave *>> travel()
    {
        currentPath.push_back(start);
        travelFrom(start);

        return allPaths;
    }


private:
    void travelFrom(const Cave* cave)
    {
        if (isEndOfTravel(cave))
        {
            allPaths.push_back(currentPath);
//            CavePathPrinter::print(currentPath);
            return;
        }
        else
        {
            for (const auto connectedCave : cave->getConnectedCaves())
            {
                if (isVisitable(connectedCave))
                {
                    currentPath.push_back(connectedCave);
                    connectedCave->visited();

                    travelFrom(connectedCave);

                    connectedCave->released();
                    currentPath.pop_back();
                }
            }
        }
    }

    bool isEndOfTravel(const Cave* cave)
    {
        return (Cave::END_CAVE == cave->TYPE);
    }

protected:
    size_t getCurrentPathLength() const
    {
        return currentPath.size();
    }

protected:
    virtual bool isVisitable(Cave* cave) = 0;

private:
    Cave* start;
    std::vector<Cave *> currentPath;
    std::vector<std::vector<Cave *>> allPaths;
};

class StarOneCaveTraveler : public CaveTraveler
{
public:
    explicit StarOneCaveTraveler(Cave* startCave) : CaveTraveler(startCave) {}

protected:
    bool isVisitable(Cave *cave) override
    {
        if (Cave::SMALL_CAVE == cave->TYPE)
        {
            return (cave->getVisitedCount() == 0);
        }

        return true;
    }
};

class StarTwoCaveTraveler : public CaveTraveler
{
public:
    explicit StarTwoCaveTraveler(Cave* startCave) : CaveTraveler(startCave) {}

protected:
    bool isVisitable(Cave *cave) override
    {
        if (smallCaveVisitedTwice != nullptr && smallCaveVisitedTwice->getVisitedCount() < 2)
        {
            smallCaveVisitedTwice = nullptr;
        }

        if (Cave::SMALL_CAVE == cave->TYPE)
        {
            if (smallCaveVisitedTwice == nullptr)
            {
                if (cave->getVisitedCount() == 1)
                {
                    smallCaveVisitedTwice = cave;
                }
                return true;
            }
            else if (smallCaveVisitedTwice == cave)
            {
                return false;
            }
            else
            {
                 return (cave->getVisitedCount() == 0);
            }
        }

        return true;
    }

private:
    Cave *smallCaveVisitedTwice = nullptr;
};


int main(){
    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    std::vector<std::unique_ptr<Cave>> caves;
    std::unordered_map<std::string, Cave *> registeredCaves;

    std::string input;
    while(!std::cin.eof())
    {
        getline(std::cin, input);
        auto inputCaveNames = InputParser::parse(input);

        for(const auto& caveName : inputCaveNames)
        {
            if (registeredCaves.find(caveName) == registeredCaves.end())
            {
                auto newCave = CaveMaker::make(caveName);
                caves.emplace_back(std::move(newCave));
                registeredCaves.emplace(caveName, caves.back().get());
            }
        }

        auto leftCave = registeredCaves[inputCaveNames[0]];
        auto rightCave =registeredCaves[inputCaveNames[1]];
        CaveConnector::connect({leftCave, rightCave});

    }

    std::unique_ptr<CaveTraveler> traveler;
    traveler.reset(new StarOneCaveTraveler(registeredCaves["start"]));
    auto startOnePaths = traveler->travel();

    traveler.reset(new StarTwoCaveTraveler(registeredCaves["start"]));
    auto startTwoPaths = traveler->travel();

//    CavePathPrinter::printAll(startTwoPaths);

    std::cout << "Day 12-1: " << startOnePaths.size() << " " << std::endl;
    std::cout << "Day 12-2: " << startTwoPaths.size() << " " << std::endl;

    return 0;
}


