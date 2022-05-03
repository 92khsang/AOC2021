#include <utility>
#include <vector>
#include <array>
#include <iostream>
#include <string>

using Scope = std::pair<int64_t, int64_t>;

struct Cube
{
    bool on{};
    std::array<Scope, 3> scopes;
    int64_t sign = 1;
};

class InputReader
{
public:
    static std::vector<Cube> read()
    {
        std::ios_base::sync_with_stdio(false);
        freopen(INPUT_FILE_PATH, "rt", stdin);

        std::vector<Cube> inputs;
        std::string input;

        while (!std::cin.eof())
        {
            getline(std::cin, input);
            if (input.empty())
            {
                continue;
            }

            Cube cube;
            char operate[4];
            sscanf(input.c_str(), "%s x=%ld..%ld,y=%ld..%ld,z=%ld..%ld",
                   operate,
                   &cube.scopes[0].first, &cube.scopes[0].second,
                   &cube.scopes[1].first, &cube.scopes[1].second,
                   &cube.scopes[2].first, &cube.scopes[2].second);

            cube.on = (std::string(operate) == "on");
            inputs.push_back(cube);
        }

        return inputs;
    }
};

class ScopeRule {
public:
    virtual bool isProcessable(const Cube& cube) const = 0;
};

class Counter {
public:
    size_t countAll() {
        total = 0;

        std::vector<Cube> cubes;
        for (const auto& initialCube : initialCubes) {
            if (!rule->isProcessable(initialCube)) {
                continue;
            }

            int cubesSize = cubes.size();
            for (int i = 0; i < cubesSize; i++) {
                const auto& prevCube = cubes[i];
                auto overlappedCube = makeOverlappedCube(prevCube, initialCube);
                auto volume = getVolume(overlappedCube);

                if (volume != 0) {
                    total += volume;
                    cubes.push_back(overlappedCube);
                }
            }

            if (initialCube.on) {
                total += getVolume(initialCube);
                cubes.push_back(initialCube);
            }


        }

        return total;
    }

private:
    Cube makeOverlappedCube(const Cube& left, const Cube& right) {
        Cube overlappedCube;
        for (int i = 0 ; i < left.scopes.size(); i++) {
            overlappedCube.scopes[i].first = std::max(left.scopes[i].first, right.scopes[i].first);
            // This is for overlap checking
            overlappedCube.scopes[i].second = std::max(std::min(left.scopes[i].second, right.scopes[i].second), overlappedCube.scopes[i].first - 1);
        }

        overlappedCube.sign = -1 * left.sign * right.sign;

        return overlappedCube;
    }

    int64_t getVolume(const Cube& cube) {
        int64_t result = cube.sign;
        for (const auto& scope : cube.scopes) {
            result *= getLength(scope);
        }
        return result;
    }

    int64_t getLength(const Scope& scope) const {
        return (scope.second - scope.first + 1);
    }

public:
    void setScopeRule(ScopeRule *scopeRule) {
        this->rule = scopeRule;
    }

    void setInputSteps(const std::vector<Cube> &inputCubes) {
        this->initialCubes = inputCubes;
    }

private:
    int64_t total;
    ScopeRule* rule;
    std::vector<Cube> initialCubes;
};

class NoScopeRule : public ScopeRule {
public:
    bool isProcessable(const Cube &cube) const override {
        return true;
    }
};

class StartOneScopeRule : public ScopeRule {
public:
    bool isProcessable(const Cube &cube) const override {
        for (const auto& scope : cube.scopes) {
            if (scope.first < -50 || scope.second > 50) {
                return false;
            }
        }
        return true;
    }
};

int main()
{

    auto inputs = InputReader::read();
    Counter counter;

    NoScopeRule noScopeRule;
    StartOneScopeRule startOneScopeRule;

    counter.setInputSteps(inputs);
    counter.setScopeRule(&startOneScopeRule);
    std::cout << "22-1: " << counter.countAll() << std::endl;

    counter.setScopeRule(&noScopeRule);
    std::cout << "22-2: " << counter.countAll() << std::endl;

}