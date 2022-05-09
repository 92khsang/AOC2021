#include <utility>
#include <vector>
#include <array>
#include <iostream>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <memory>
#include <queue>

using Map = std::vector<std::string>;

struct Resident {
    int movingCost;
    int roomColumnIndex;
};

static const struct {
    std::unordered_map<char, Resident> residents = {
            {'A', Resident{1, 3}},
            {'B', Resident{10, 5}},
            {'C', Resident{100, 7}},
            {'D', Resident{1000, 9}}
    };

    bool isResident(char c) const {
        return (residents.find(c) != residents.end());
    }
} RESIDENT_MAP;

static const int HALLWAY_ROW = 1;
static const int ROOM_ENTRANCE_ROW = 2;

static const struct {
    int hallwayRow = 1;
    int roomEntranceRow = 2;
} FIXED_MAP_INFO;

class Point {
public:
    Point() = default;
    Point(int row, int col) : row(row), col(col) {}

    bool operator==(const Point &rhs) const {
        return row == rhs.row &&
               col == rhs.col;
    }

    bool operator!=(const Point &rhs) const {
        return !(rhs == *this);
    }

public:
    int row, col;
};

class MapMaker
{
public:
    static Map makeInputMap()
    {
        std::ios_base::sync_with_stdio(false);
        freopen(INPUT_FILE_PATH, "rt", stdin);

        Map map;
        std::string input;

        while (!std::cin.eof())
        {
            getline(std::cin, input);
            if (input.empty())
            {
                continue;
            }

            map.push_back(input);
        }

        return map;
    }

    static Map makeFinalMapFrom(const Map& inputMap) {
        Map finalMap = inputMap;

        const int DEEPEST_ROOM_ROW = inputMap.size() - 2;

        for (const auto& resident : RESIDENT_MAP.residents) {
            const auto& roomCol = resident.second.roomColumnIndex;
            for (int roomRow = FIXED_MAP_INFO.roomEntranceRow ; roomRow <= DEEPEST_ROOM_ROW; roomRow++) {
                finalMap[roomRow][roomCol] = resident.first;
            }
        }

        return finalMap;
    }
};


struct point_hash {
    std::size_t operator() (const Point& p) const {
        return (p.row << 2) + p.col;
    }
};

class DestinationFinder {
public:
    virtual bool isApplicable() const = 0;
    virtual std::vector<Point> find() = 0;

    void setMap(const Map &currentMap) {
        map = currentMap;
    }

    void setPoint(const Point &startPoint) {
        point = startPoint;
    }

protected:
    bool isHallwayToDestBlocked(const int destColmn) {
        for (int hallwayCol = std::min(destColmn, point.col) + 1; hallwayCol < std::max(destColmn, point.col); hallwayCol++) {
            if (map[HALLWAY_ROW][hallwayCol] != '.') {
                return true;
            }
        }

        return false;
    }

protected:
    Map map;
    Point point;
};

class H2RDestinationFinder : public DestinationFinder {
public:
    bool isApplicable() const override {
        return point.row == HALLWAY_ROW;
    }

    std::vector<Point> find() override {
        movingResident = map[point.row][point.col];
        destRoomColumn = RESIDENT_MAP.residents.find(movingResident)->second.roomColumnIndex;

        if (isHallwayToDestBlocked(destRoomColumn)) {
            return {};
        }

        if (isRoomIsFull() || areOtherResidentsStaying()) {
            return {};
        }

        auto emptyRow = findEmptyRowInRoom();

        return {Point(emptyRow, destRoomColumn)};
    }

private:
    bool isRoomIsFull() {
        return map[ROOM_ENTRANCE_ROW][destRoomColumn] != '.';
    }

    bool areOtherResidentsStaying() {
        const int DEEPEST_ROOM_ROW = map.size() - 2;

        for (int row = ROOM_ENTRANCE_ROW; row <= DEEPEST_ROOM_ROW; row++) {
            char charInPosition = map[row][destRoomColumn];
            if (charInPosition != '.' && charInPosition != movingResident) {
                return true;
            }
        }

        return false;
    }

    int findEmptyRowInRoom() {
        const int DEEPEST_ROOM_ROW = map.size() - 2;

        int row;
        for (row = DEEPEST_ROOM_ROW; row >= ROOM_ENTRANCE_ROW; row--) {
            if (map[row][destRoomColumn] == '.') {
                break;
            }
        }

        return row;
    }

private:
    char movingResident;
    int destRoomColumn;
};

class R2HDestinationFinder : public DestinationFinder {
public:
    bool isApplicable() const override {
        return point.row != HALLWAY_ROW;
    }

    std::vector<Point> find() override {
        std::vector<Point> destinations;

        for (int col = 1 ; col < map[HALLWAY_ROW].size(); col++) {
            if (isHallayInFrontOfRoom(col)) {
                continue;
            }

            if (map[HALLWAY_ROW][col] == '.') {
                if (!isHallwayToDestBlocked(col)) {
                    destinations.emplace_back(HALLWAY_ROW, col);
                }
            }
        }

        return destinations;
    }
private:
    bool isHallayInFrontOfRoom(int column) {
        for (const auto& resident : RESIDENT_MAP.residents) {
            if (column == resident.second.roomColumnIndex) {
                return true;
            }
        }

        return false;
    }
};

class MovableResidentPositionFinder {
public:
    explicit MovableResidentPositionFinder(Map map) : map(std::move(map)) {}

    std::vector<Point> findAll() {
        findInHallway();
        findInRoom();

        return positions;
    }

private:
    void findInHallway() {
        for (int col = 1; col < map[HALLWAY_ROW].size() - 1; col++) {
            const char charInPosition = map[HALLWAY_ROW][col];
            if (RESIDENT_MAP.isResident(charInPosition)) {
                positions.emplace_back(HALLWAY_ROW, col);
            }
        }

    }

    void findInRoom() {
        const int DEEPEST_ROOM_ROW = map.size() - 2;

        for (const auto& residentInfo : RESIDENT_MAP.residents) {
            auto roomColumnIndex = residentInfo.second.roomColumnIndex;

            for (int row = ROOM_ENTRANCE_ROW; row <= DEEPEST_ROOM_ROW; row++) {
                if (map[row][roomColumnIndex] != '.') {
                    positions.emplace_back(row, roomColumnIndex);
                    break;
                }
            }
        }
    }

private:
    Map map;
    std::vector<Point> positions;
};

class MapMover{
public:
    MapMover(const Map &map, const Point &residentPosition, const Point &dest) : map(map),
                                                                                 residentPosition(residentPosition),
                                                                                 dest(dest) {}

    Map move() {
        Map swappedMap = map;
        std::swap(swappedMap[residentPosition.row][residentPosition.col], swappedMap[dest.row][dest.col]);
        return swappedMap;
    }

    int getCost() {
        auto rowMove = std::abs(dest.row - residentPosition.row);
        auto colMove = std::abs(dest.col - residentPosition.col);

        auto resident = map[residentPosition.row][residentPosition.col];
        auto residentInfo = RESIDENT_MAP.residents.find(resident)->second;
        auto movingCost = residentInfo.movingCost;

        return (rowMove + colMove) * movingCost;
    }

private:
    const Map& map;
    const Point& residentPosition;
    const Point& dest;
};


class LowestCostExplorer {
private:
    using Cost = int;
    using CostWithHeuristic = int;
    using Node = std::tuple<Map, Cost, CostWithHeuristic>;
    using Nodes = std::vector<Node>;

public:
    LowestCostExplorer() {
        destinationFinders.emplace_back(new H2RDestinationFinder());
        destinationFinders.emplace_back(new R2HDestinationFinder());
    }

    int findLowestCost() {
        clear();

        queue.push(std::make_tuple(originalMap, 0, heuristic(originalMap)));
        Nodes n;

        while (!queue.empty()) {
            Node topNode = queue.top();
            Map currentMap = std::get<0>(topNode);
            Cost currentCost = std::get<1>(topNode);
            queue.pop();

            MovableResidentPositionFinder residentPositionFinder(currentMap);

            auto movableResidentPositions = residentPositionFinder.findAll();
            for (const auto& movableResidentPosition : movableResidentPositions) {
                std::vector<Point> destCandidates;

                for (const auto& destinationFinder : destinationFinders) {
                    destinationFinder->setMap(currentMap);
                    destinationFinder->setPoint(movableResidentPosition);

                    if (destinationFinder->isApplicable()) {
                        destCandidates = destinationFinder->find();

                        for (const auto& destCandidate : destCandidates) {
                            MapMover mover(currentMap, movableResidentPosition, destCandidate);

                            auto movedMap = mover.move();

                            if (isAlreadyChecked(movedMap)) {
                                continue;
                            }

                            auto movingCost = mover.getCost();
                            auto newCost = currentCost + movingCost;
                            auto newNode = std::make_tuple(movedMap, newCost, newCost + heuristic(movedMap));

                            if (movedMap == finalMap) {
                                return newCost;
                            }

                            queue.push(newNode);
                            snapshot.insert(movedMap);
                        }
                    }
                }
            }
        }

        return -1;
    }

    void setOriginalMap(const Map &inputMap) {
        this->originalMap = inputMap;
        this->finalMap = MapMaker::makeFinalMapFrom(inputMap);
    }

private:
    void clear() {
        snapshot = std::unordered_set<Map, map_hash>();
        queue = std::priority_queue<Node, Nodes, node_compare>();
    }

    bool isAlreadyChecked(const Map& map) {
        return snapshot.find(map) != snapshot.end();
    }

private:
    int heuristic(const Map& map) {
        int weight = 0;

        std::unordered_map<char, std::vector<Point>> residentPositions;
        for (int row = 1 ; row < map.size() - 1; row++) {
            for (int col = 1; col < map[0].size() - 1; col++) {
                auto charInPoint = map[row][col];
                if (RESIDENT_MAP.isResident(charInPoint)) {
                    residentPositions[charInPoint].push_back(Point(row, col));
                }
            }
        }

        const int DEEPEST_ROOM_ROW = map.size() - 2;

        for (const auto& residentPosition : residentPositions) {
            char resident = residentPosition.first;
            int roomColumnIndex = RESIDENT_MAP.residents.find(resident)->second.roomColumnIndex;
            int movingCost =  RESIDENT_MAP.residents.find(resident)->second.movingCost;

            auto& positions = residentPositions.find(resident)->second;

            for (int row = DEEPEST_ROOM_ROW; row >= ROOM_ENTRANCE_ROW; row--) {
                Point targetPoint(row, roomColumnIndex);

                int minDistance = INT32_MAX;
                int minPositionIndex = 0;
                for (int i = 0 ; i < positions.size(); i++) {
                    const auto& position = positions[i];
                    int distance = std::abs(position.col - targetPoint.col);
                    if (distance == 0) {
                        distance += std::abs(position.row - targetPoint.row);
                    } else {
                        distance += (position.row - HALLWAY_ROW) + (targetPoint.row - HALLWAY_ROW);
                    }

                    if (distance < minDistance) {
                        minDistance = distance;
                        minPositionIndex = i;
                    }
                }

                weight += minDistance * movingCost;
                positions.erase(positions.begin() + minPositionIndex);
            }

        }


        return weight;
    }

    struct map_hash {
        std::size_t operator () (const Map& map) const {
            std::string oneRowMap;
            for (const auto& row : map) {
                oneRowMap += row;
            }
            return std::hash<std::string>()(oneRowMap);
        }
    };

    struct node_compare {
        bool operator() (const Node& l, const Node& r) const {
            return std::get<2>(l) > std::get<2>(r);
        }
    };

private:
    Map originalMap;
    Map finalMap;

    std::unordered_set<Map, map_hash> snapshot;
    std::priority_queue<Node, Nodes, node_compare> queue;

    std::vector<std::unique_ptr<DestinationFinder>> destinationFinders;
};

int main()
{
    auto inputMap = MapMaker::makeInputMap();

    LowestCostExplorer explorer;
    explorer.setOriginalMap(inputMap);

    auto lowestCost = explorer.findLowestCost();

    std::cout << "23-1: " << lowestCost << std::endl;

    std::string start2Line4 = "  #D#C#B#A#";
    std::string start2Line5 = "  #D#B#A#C#";

    inputMap.insert(inputMap.begin() + 3, start2Line4);
    inputMap.insert(inputMap.begin() + 4, start2Line5);

    explorer.setOriginalMap(inputMap);

    lowestCost = explorer.findLowestCost();

    std::cout << "23-2: " << lowestCost << std::endl;
}