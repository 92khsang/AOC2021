#include <bits/stdc++.h>
#include <vector>
#include <map>

bool IsCrapExist(const std::map<uint32_t, size_t>& crapCounts, uint32_t alignPosition)
{
    return (crapCounts.find(alignPosition) != crapCounts.end());
}

size_t getLeftIncreaseNumber(const std::map<uint32_t, size_t>& crapCounts, uint32_t alignPosition)
{
    size_t increase = 0;

    for (const auto& crapCount : crapCounts)
    {
        uint32_t crapPosition = crapCount.first;
        size_t targetPosCrapCount = crapCount.second;

        if (crapPosition >= alignPosition)
        {
            break;
        }

        increase += (alignPosition - crapPosition) * targetPosCrapCount;
    }

    return increase;
}

size_t getRightDecreaseNumber(const std::map<uint32_t, size_t>& crapCounts, uint32_t prevAlignPosition)
{
    size_t decrease = 0;

    for (auto crapCount = crapCounts.rbegin(); crapCount != crapCounts.rend(); ++crapCount)
    {
        uint32_t crapPosition = crapCount->first;
        size_t targetPosCrapCount = crapCount->second;

        if (crapPosition <= prevAlignPosition)
        {
            break;
        }

        decrease += (crapPosition - prevAlignPosition) * targetPosCrapCount;
    }

    return decrease;
}

int main()
{

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    std::string line;
    std::size_t fuel = 0;
    std::size_t totalCrapsCount = 0;
    std::uint32_t farthestCrapLocation = 0;
    std::map<uint32_t, size_t> crapCounts;

    std::getline(std::cin, line);
    std::stringstream inputStream(line);

    for (uint32_t i; inputStream >> i;)
    {
        crapCounts[i]++;
        totalCrapsCount++;

        if (i > farthestCrapLocation)
        {
            farthestCrapLocation = i;
        }

        if (inputStream.peek() == ',')
            inputStream.ignore();
    }

    for (const auto& crapCount : crapCounts)
    {
        uint32_t crapPosition = crapCount.first;
        size_t targetPosCrapCount = crapCount.second;
        size_t targetFuel = 0;

        for (size_t i = 1 ; i <= crapPosition; i++)
        {
            targetFuel+= i;
        }

        fuel += (targetFuel * targetPosCrapCount);
    }


    for (uint32_t alignPosition = 1; alignPosition <= farthestCrapLocation; alignPosition++)
    {
        size_t nextFuel = (fuel + getLeftIncreaseNumber(crapCounts, alignPosition)) - (getRightDecreaseNumber(crapCounts, alignPosition - 1));

        if (fuel < nextFuel)
        {
            break;
        }

        fuel = nextFuel;
    }

    std::cout << "Day 07-2: " << fuel << " " << std::endl;

    return 0;
}


