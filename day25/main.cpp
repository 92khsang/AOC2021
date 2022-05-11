#include <vector>
#include <iostream>
#include <string>
#include <memory>

struct Region {
    char type;
    Region* right;
    Region* down;
};

class SeeCucumber {
public:
    SeeCucumber(Region *region) : region(region) {}

    virtual bool isMovable() const = 0;
    virtual void move() = 0;

protected:
    void swap(Region* dest) {
        Region* src = region;

        char tempType = src->type;
        src->type = dest->type;
        dest->type = tempType;

        region = dest;
    }
protected:
    Region* region;
};

class RightMovingSeeCucumber : public SeeCucumber {
public:
    RightMovingSeeCucumber(Region *region) : SeeCucumber(region) {}

    bool isMovable() const override {
        return region->right->type == '.';
    }

    void move() override {
        swap(region->right);
    }
};

class DownMovingSeeCucumber : public SeeCucumber {
public:
    DownMovingSeeCucumber(Region *region) : SeeCucumber(region) {}

    bool isMovable() const override {
        return region->down->type == '.';
    }

    void move() override {
        swap(region->down);
    }
};

using Regions = std::vector<Region>;
using RegionMap = std::vector<Regions>;

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

            inputs.push_back(std::move(input));
        }

        return inputs;
    }
};

class RegionMapMaker {
public:
    static std::unique_ptr<RegionMap> make(const std::vector<std::string>& inputs) {
        std::unique_ptr<RegionMap> map(new RegionMap());

        for (const auto& input : inputs) {
            map->push_back({});
            auto& regions = map->back();
            for (const char& c : input) {
                Region region{};
                region.type = c;
                regions.push_back(region);
            }
        }

        for (int row = 0; row < map->size(); row++) {
            auto& regions = map->at(row);
            for (int col = 0; col < regions.size(); col++) {
                auto& region = regions[col];
                if (col < regions.size() - 1) {
                    region.right = &(map->at(row)[col + 1]);
                } else {
                    region.right = &(map->at(row)[0]);
                }

                if (row < map->size() - 1) {
                    region.down = &(map->at(row + 1)[col]);
                } else {
                    region.down = &(map->at(0)[col]);
                }
            }
        }

        return std::move(map);
    }
};

class SeeCucumberMover {
public:
    explicit SeeCucumberMover(const std::vector<std::string>& inputs)
        : totalMovingCnt(0) {
        regionMap = RegionMapMaker::make(inputs);

        for (auto& regions : *regionMap) {
            for (auto& region : regions) {
                if (region.type == '>') {
                    rightMovingSeeCucumbers.emplace_back(new RightMovingSeeCucumber(&region));
                } else if (region.type == 'v') {
                    downMovingSeeCucumbers.emplace_back(new DownMovingSeeCucumber(&region));
                }
            }
        }
    }

    size_t move() {
        std::vector<SeeCucumber *> rightMovableSeeCucumbers = findMovableSeeCucumbers(rightMovingSeeCucumbers);
        for (auto& movableSeeCucumber : rightMovableSeeCucumbers) {
            movableSeeCucumber->move();
        }

        std::vector<SeeCucumber *> downMovableSeeCucumbers = findMovableSeeCucumbers(downMovingSeeCucumbers);
        for (auto& movableSeeCucumber : downMovableSeeCucumbers) {
            movableSeeCucumber->move();
        }

        totalMovingCnt++;
        return rightMovableSeeCucumbers.size() + downMovableSeeCucumbers.size();
    }

    size_t getTotalMovingCnt() const {
        return totalMovingCnt;
    }

    void show() {
        for (auto & regions : *regionMap) {
            for (auto & region : regions) {
                std::cout << region.type;
            }
            std::cout << std::endl;
        }
    }

private:
    std::vector<SeeCucumber *> findMovableSeeCucumbers(std::vector<std::unique_ptr<SeeCucumber>>& seeCucumbers) {
        std::vector<SeeCucumber *> movableSeeCucumbers;
        for (auto& seeCucumber : seeCucumbers) {
            if (seeCucumber->isMovable()) {
                movableSeeCucumbers.push_back(seeCucumber.get());
            }
        }
        return movableSeeCucumbers;
    }

private:
    size_t totalMovingCnt;

    std::unique_ptr<RegionMap> regionMap;

    std::vector<std::unique_ptr<SeeCucumber>> rightMovingSeeCucumbers;
    std::vector<std::unique_ptr<SeeCucumber>> downMovingSeeCucumbers;
};

int main()
{
    auto inputs = InputReader::read();
    SeeCucumberMover mover(inputs);

    while (mover.move() != 0) {}
    std::cout << "25-1: " << mover.getTotalMovingCnt() << std::endl;
}