#include <bits/stdc++.h>
#include <vector>
#include <map>

int main(){

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
        fuel += i;
        totalCrapsCount++;

        if (i > farthestCrapLocation)
        {
            farthestCrapLocation = i;
        }

        if (inputStream.peek() == ',')
            inputStream.ignore();
    }

    size_t leftPosCrapCounts = 0;
    size_t currentPosCrapCounts = crapCounts[0];
    size_t rightPosCrapCounts = totalCrapsCount - currentPosCrapCounts;

    for (size_t alignPosition = 1; alignPosition <= farthestCrapLocation; alignPosition++)
    {
        size_t prevTargetPostCrapCounts = currentPosCrapCounts;
        currentPosCrapCounts = crapCounts[alignPosition];
        leftPosCrapCounts += prevTargetPostCrapCounts;
        rightPosCrapCounts -= currentPosCrapCounts;

        size_t nextFuel = (fuel + leftPosCrapCounts) - (rightPosCrapCounts + currentPosCrapCounts);

        if (fuel < nextFuel)
        {
            break;
        }

        fuel = nextFuel;
    }

    std::cout << "Day 07-1: " << fuel << " " << std::endl;

    return 0;
}


