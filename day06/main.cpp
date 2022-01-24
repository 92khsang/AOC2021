#include <bits/stdc++.h>
#include <vector>
#include <unordered_map>

class Fish
{
public:
    friend struct fish_hash;

    const size_t BIRTH_DELAY = 7;
    const size_t NEW_FISH_BIRTH_DELAY = 8;

    explicit Fish(size_t initialBirthDay, size_t initialLeftDateOfBirth)
            : birthDay(initialBirthDay), leftDateOfBirth(initialLeftDateOfBirth) {}

    Fish giveBirth(size_t numOfChildren) const
    {
        return Fish(birthDay + (leftDateOfBirth + 1) + (numOfChildren * BIRTH_DELAY), NEW_FISH_BIRTH_DELAY);
    }

    size_t getBirthDay() const
    {
        return birthDay;
    }

    bool operator==(const Fish& other) const
    {
        return (birthDay == other.birthDay && leftDateOfBirth == other.leftDateOfBirth);
    }

private:
    size_t birthDay;
    size_t leftDateOfBirth;
};

struct fish_hash
{
    std::size_t operator() (const Fish &fish) const {
        return std::hash<size_t>()(fish.birthDay) ^ std::hash<size_t>()(fish.leftDateOfBirth);
    }
};

class FishMemory
{
public:
    bool isDataExist(const Fish& fish) const
    {
        return (memory.find(fish) != memory.end());
    }

    size_t getBreedableCount(const Fish& fish) const
    {
        return memory.find(fish)->second;
    }

    void setBreadableCount(const Fish& fish, size_t breableCount)
    {
        memory[fish] = breableCount;
    }

private:
    std::unordered_map<Fish, size_t, fish_hash> memory;
};

class FishCounter
{
public:
    explicit FishCounter(size_t initialFishCount) : totalFishCount(initialFishCount){};

    void addFish(const Fish& fish)
    {
        totalFishCount += getTotalChildrenFish(fish);
    }

    size_t getTotal() const
    {
        return totalFishCount;
    }

private:
    const size_t LAST_DAY_OF_BIRTH = 256;

    size_t getTotalChildrenFish(const Fish& fish)
    {
        if (memory.isDataExist(fish))
        {
            return memory.getBreedableCount(fish);
        }

        size_t childrenCount = 0;
        size_t totalDescendant = 0;

        while(true)
        {
            Fish newFish = fish.giveBirth(childrenCount);
            if (newFish.getBirthDay() > LAST_DAY_OF_BIRTH)
            {
                break;
            }

            childrenCount++;
            totalDescendant += getTotalChildrenFish(newFish);
        }


        memory.setBreadableCount(fish, childrenCount + totalDescendant);
        return (childrenCount + totalDescendant);
    }

    size_t totalFishCount;
    FishMemory memory{};

};


int main(){


    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    std::string line;
    std::vector<Fish> firstFishes;

    std::getline(std::cin, line);
    std::stringstream inputStream(line);

    for (uint32_t i; inputStream >> i;)
    {
        firstFishes.emplace_back(0, i);
        if (inputStream.peek() == ',')
            inputStream.ignore();
    }

    size_t initialFishCount = firstFishes.size();
    FishCounter counter(initialFishCount);

    for (const auto& fish : firstFishes)
    {
        counter.addFish(fish);
    }

    std::cout << "Day 06: " << counter.getTotal() << " " << std::endl;

    return 0;
}


