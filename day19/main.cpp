#include <vector>
#include <array>
#include <iostream>
#include <string>
#include <memory>
#include <algorithm>
#include <cassert>

class Vector
{
public:
    Vector(int x, int y, int z) : x(x), y(y), z(z) {}

    bool operator==(const Vector &rhs) const {
        return x == rhs.x &&
               y == rhs.y &&
               z == rhs.z;
    }

    bool operator!=(const Vector &rhs) const {
        return !(rhs == *this);
    }

    Vector operator-(const Vector& rhs) const {
        return {x - rhs.x, y - rhs.y, z - rhs.z};
    }

    Vector operator+(const Vector& rhs) const {
        return {x + rhs.x, y + rhs.y, z + rhs.z};
    }

    int16_t x;
    int16_t y;
    int16_t z;
};

class Beacon
{
public:
    explicit Beacon(const Vector& vector) : vector(vector) {}
    Beacon(int x, int y, int z) : vector(x, y, z) {}
    Beacon() : Beacon(0, 0, 0) {}

    bool operator==(const Beacon &rhs) const {
        return vector == rhs.vector;
    }

    bool operator!=(const Beacon &rhs) const {
        return !(rhs == *this);
    }

    Vector vector;
};

std::vector<Vector (*)(Vector const&)> rotateFuncs{
        [](Vector const& p) { return Vector{ p.x, p.y, p.z}; },
        [](Vector const& p) { return Vector{ p.x, -p.z, p.y}; },
        [](Vector const& p) { return Vector{ p.x, -p.y, -p.z}; },
        [](Vector const& p) { return Vector{ p.x, p.z, -p.y}; },
        [](Vector const& p) { return Vector{-p.y, p.x, p.z}; },
        [](Vector const& p) { return Vector{ p.z, p.x, p.y}; },
        [](Vector const& p) { return Vector{ p.y, p.x, -p.z}; },
        [](Vector const& p) { return Vector{-p.z, p.x, -p.y}; },
        [](Vector const& p) { return Vector{-p.x, -p.y, p.z}; },
        [](Vector const& p) { return Vector{-p.x, -p.z, -p.y}; },
        [](Vector const& p) { return Vector{-p.x, p.y, -p.z}; },
        [](Vector const& p) { return Vector{-p.x, p.z, p.y}; },
        [](Vector const& p) { return Vector{ p.y, -p.x, p.z}; },
        [](Vector const& p) { return Vector{ p.z, -p.x, -p.y}; },
        [](Vector const& p) { return Vector{-p.y, -p.x, -p.z}; },
        [](Vector const& p) { return Vector{-p.z, -p.x, p.y}; },
        [](Vector const& p) { return Vector{-p.z, p.y, p.x}; },
        [](Vector const& p) { return Vector{ p.y, p.z, p.x}; },
        [](Vector const& p) { return Vector{ p.z, -p.y, p.x}; },
        [](Vector const& p) { return Vector{-p.y, -p.z, p.x}; },
        [](Vector const& p) { return Vector{-p.z, -p.y, -p.x}; },
        [](Vector const& p) { return Vector{-p.y, p.z, -p.x}; },
        [](Vector const& p) { return Vector{ p.z, p.y, -p.x}; },
        [](Vector const& p) { return Vector{ p.y, -p.z, -p.x}; }
};

class BeaconDistance
{
public:
    BeaconDistance(const Beacon* lhs, const Beacon* rhs) :
    leftBeacon(lhs),
    rightBeacon(rhs),
    vector((lhs->vector.x - rhs->vector.x), (lhs->vector.y - rhs->vector.y), (lhs->vector.z - rhs->vector.z)) {
        for (const auto& rotateFunc : rotateFuncs)
        {
            rotatedVectors.push_back(rotateFunc(vector));
        }
    }

    bool operator==(const BeaconDistance &rhs) const {
       return (rotatedVectors.end() != (std::find(rotatedVectors.begin(), rotatedVectors.end(), rhs.vector)));
    }

    bool operator!=(const BeaconDistance &rhs) const {
        return !(rhs == *this);
    }

    const Beacon* leftBeacon;
    const Beacon* rightBeacon;

    Vector vector;
    std::vector<Vector> rotatedVectors;
};

class Scanner
{
public:
    void addBeacon(const Beacon& beacon)
    {
        addBeacon(beacon.vector.x, beacon.vector.y, beacon.vector.z);
    }

    void addBeacon(int16_t x, int16_t y, int16_t z)
    {
        beaconContainer.emplace_back(new Beacon(x, y, z));
        beacons.push_back(beaconContainer.back().get());
        const auto& newBeacon = beacons.back();
        for (const auto& beacon : beacons)
        {
            if (&beacon == &newBeacon)
            {
                continue;
            }

            distances.emplace_back(beacon, newBeacon);
            distances.emplace_back(newBeacon, beacon);
        }
    }

    int16_t number;
    std::vector<const Beacon *> beacons;
    std::vector<BeaconDistance> distances;

private:
    std::vector<std::unique_ptr<Beacon>> beaconContainer;
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
            inputs.emplace_back(input);
        }

        return inputs;
    }
};

class InputParse
{
public:
    static std::vector<std::shared_ptr<Scanner>> parse(const std::vector<std::string>& inputs)
    {
        std::vector<std::shared_ptr<Scanner>> scanners;

        for (const auto& line : inputs)
        {
            if (line.empty())
            {
                continue;
            }

            if (line.find("scanner") != std::string::npos)
            {
                int16_t scannerNumber = -1;
                sscanf(line.c_str(), "--- scanner %hd ---", &scannerNumber);
                scanners.emplace_back(new Scanner());
                scanners.back()->number = scannerNumber;
            }
            else
            {
                int16_t x, y, z;
                sscanf(line.c_str(), "%hd,%hd,%hd", &x, &y, &z);
                auto& lastScanner = scanners.back();
                lastScanner->addBeacon(x, y, z);
            }
        }

        return std::move(scanners);
    }
};

struct RotatedScanner
{
public:
    RotatedScanner() : vector(0, 0, 0) {
    }

    Vector vector;

    std::vector<Beacon> beacons;
};

class ScannerRotater
{
public:
    void setStandardScanner(Scanner* scanner)
    {
        standardScanner = scanner;
    }

    RotatedScanner rotate(Scanner *relativeScanner)
    {
        RotatedScanner rotatedScanner;

        auto firstMatchingDistanceSet = findFirstMatchingDistance(relativeScanner);

        auto targetStandardBeaconDistance = firstMatchingDistanceSet.first;
        auto anotherMatchingDistanceSet = findAnotherMatchingDistance(targetStandardBeaconDistance, relativeScanner);

        auto targetBeaconInStandard = targetStandardBeaconDistance.leftBeacon;
        auto targetBeaconInRelative  = findSameBeacon(firstMatchingDistanceSet.second, anotherMatchingDistanceSet.second);

        auto sameBeaconSet = findSameBeaconSet(targetBeaconInStandard, targetBeaconInRelative, relativeScanner);
        if (sameBeaconSet.size() < 12)
        {
            return rotatedScanner;
        }

        auto standPivot = sameBeaconSet.front().first;
        auto relativePivot = sameBeaconSet.front().second;
        int i = 1;
        for (i = 1 ; i < sameBeaconSet.size(); i++)
        {
            auto compare = sameBeaconSet[i].first;
            BeaconDistance standDistance(standPivot, compare);

            if (!(anyOfPairIsSame(standDistance) || anyOfAxisHasZeror(standDistance)))
            {
                break;
            }
        }

        BeaconDistance standDistance(standPivot, sameBeaconSet[i].first);
        BeaconDistance relativeDistance(relativePivot, sameBeaconSet[i].second);

        auto rotatedBeacon = rotateBeacon(standDistance, relativeDistance, {relativeDistance.leftBeacon});

        rotatedScanner.vector = targetBeaconInStandard->vector - rotatedBeacon[0].vector;
        rotatedScanner.beacons = rotateBeacon(standDistance, relativeDistance, relativeScanner->beacons);

        return rotatedScanner;
    }

    bool hasMatchingDistance(Scanner *relativeScanner)
    {
        for (const auto& standardDistance : standardScanner->distances)
        {
            auto relativeDistances = relativeScanner->distances;
            auto relativeDistance = std::find(relativeDistances.begin(), relativeDistances.end(), standardDistance);
            if (relativeDistance != relativeDistances.end())
            {
                return true;
            }
        }

        return false;
    }

private:
    std::pair<BeaconDistance, BeaconDistance> findFirstMatchingDistance(Scanner *relativeScanner)
    {
        for (const auto& standardDistance : standardScanner->distances)
        {
            auto relativeDistances = relativeScanner->distances;
            auto relativeDistance = std::find(relativeDistances.begin(), relativeDistances.end(), standardDistance);
            if (relativeDistance != relativeDistances.end())
            {
                return {standardDistance, *relativeDistance};
            }
        }

        assert(false);
    }


    std::pair<BeaconDistance, BeaconDistance> findAnotherMatchingDistance(const BeaconDistance& targetStandardBeaconDistance, Scanner *relativeScanner)
    {
        for (const auto& standardDistance : standardScanner->distances)
        {
            if (!(standardDistance.leftBeacon == targetStandardBeaconDistance.leftBeacon ||
                standardDistance.leftBeacon == targetStandardBeaconDistance.rightBeacon ||
                standardDistance.rightBeacon == targetStandardBeaconDistance.leftBeacon ||
                standardDistance.rightBeacon == targetStandardBeaconDistance.rightBeacon))
            {
                continue;
            }

            auto relativeDistances = relativeScanner->distances;
            auto relativeDistance = std::find(relativeDistances.begin(), relativeDistances.end(), standardDistance);
            if (relativeDistance != relativeDistances.end())
            {
                return {standardDistance, *relativeDistance};
            }
        }

        assert(false);
    }

    std::vector<std::pair<const Beacon *,const Beacon *>> findSameBeaconSet(const Beacon* standardPivotBeacon, const Beacon* relativePivotBeacon, Scanner *relativeScanner)
    {
        std::vector<std::pair<const Beacon *, const Beacon *>> sameBeacons;
        sameBeacons.emplace_back(standardPivotBeacon, relativePivotBeacon);

        for (const auto& standardDistance : standardScanner->distances)
        {
            if (standardDistance.leftBeacon != standardPivotBeacon && standardDistance.rightBeacon != standardPivotBeacon)
            {
                continue;
            }

            if (standardDistance.leftBeacon->vector.x == -537 || standardDistance.rightBeacon->vector.x == -537)
            {
                printf("here\n");
            }

            auto relativeDistances = relativeScanner->distances;
            auto relativeDistance = std::find(relativeDistances.begin(), relativeDistances.end(), standardDistance);
            if (relativeDistance != relativeDistances.end())
            {
                auto anotherRelativeBeacon = findDiffBeacon(*relativeDistance, relativePivotBeacon);
                auto anotherStandBeacon = findDiffBeacon(standardDistance, standardPivotBeacon);
                sameBeacons.emplace_back(anotherStandBeacon, anotherRelativeBeacon);
            }
        }

        return sameBeacons;
    }

    std::vector<Beacon> rotateBeacon(const BeaconDistance& standardBeaconDistance, const BeaconDistance& relativeBeaconDistance, const std::vector<const Beacon *>& unrotatedBeacons)
    {
        std::vector<Beacon> rotatedBeacons;

        int rotateFuncIndex = -1;

        for (int i = 0 ; i < rotateFuncs.size(); i++)
        {
            if (rotateFuncs[i](relativeBeaconDistance.vector) == standardBeaconDistance.vector)
            {
                rotateFuncIndex = i;
                break;
            }
        }

        rotatedBeacons.reserve(unrotatedBeacons.size());
        for (auto unrotatedBeacon : unrotatedBeacons)
        {
            rotatedBeacons.emplace_back(rotateFuncs[rotateFuncIndex](unrotatedBeacon->vector));
        }

        return rotatedBeacons;
    }

    const Beacon * findSameBeacon(const BeaconDistance& lhs, const BeaconDistance& rhs)
    {
        if (lhs.leftBeacon == rhs.leftBeacon || lhs.leftBeacon == rhs.rightBeacon)
        {
            return lhs.leftBeacon;
        }
        else
        {
            return lhs.rightBeacon;
        }
    }

    const Beacon * findDiffBeacon(const BeaconDistance& lhs, const Beacon* rhs)
    {
        if (lhs.leftBeacon == rhs)
        {
            return lhs.rightBeacon;
        }
        else
        {
            return lhs.leftBeacon;
        }
    }

    bool anyOfPairIsSame(const BeaconDistance& distance) const
    {
        return (abs(distance.vector.x) == abs(distance.vector.y)) || (abs(distance.vector.x) == abs(distance.vector.z)) || (abs(distance.vector.z) == abs(distance.vector.y));
    }

    bool anyOfAxisHasZeror(const BeaconDistance& distance) const
    {
        return (distance.vector.x == 0) || (distance.vector.y == 0) || (distance.vector.z == 0);
    }


private:
    Scanner *standardScanner;
};


int main()
{
    auto inputs = InputReader::read();
    auto scanners  = InputParse::parse(inputs);

    ScannerRotater rotater;
    std::vector<std::shared_ptr<Scanner>> rotatedScanners;

    rotatedScanners.emplace_back(scanners[0]);
    scanners.erase(scanners.begin());

    std::vector<Vector> scannerVectors;
    scannerVectors.emplace_back(0, 0, 0);

    while (!scanners.empty())
    {
        for (int i = 0 ; i < scanners.size(); i++)
        {
            auto& unprocessedScanner = scanners[i];
            bool isUpdated = false;
            for (int j =  0 ; j < rotatedScanners.size(); j++)
            {
                auto& rotatedScanner = rotatedScanners[j];
                rotater.setStandardScanner(rotatedScanner.get());

                if (rotater.hasMatchingDistance(unprocessedScanner.get()))
                {
                    auto rotatedUnprocessedScanner = rotater.rotate(unprocessedScanner.get());
                    if (rotatedUnprocessedScanner.beacons.empty())
                    {
                        continue;
                    }
//                    printf("[%d] %d,%d,%d\n", unprocessedScanner->number, rotatedUnprocessedScanner.vector.x, rotatedUnprocessedScanner.vector.y, rotatedUnprocessedScanner.vector.z);
                    scannerVectors.emplace_back(rotatedUnprocessedScanner.vector);

                    std::shared_ptr<Scanner> newRotatedScanner(new Scanner());
                    newRotatedScanner->number = unprocessedScanner->number;
                    for (const auto& rotatedBeacon : rotatedUnprocessedScanner.beacons)
                    {
                        Beacon rotatedBeaconPerspectiveByStandard(rotatedBeacon.vector + rotatedUnprocessedScanner.vector);
                        newRotatedScanner->addBeacon(rotatedBeaconPerspectiveByStandard);
                    }

                    rotatedScanners.push_back(newRotatedScanner);
                    scanners.erase(scanners.begin() + i);
                    isUpdated = true;
                    break;
                }
            }

            if (isUpdated)
            {
                break;
            }
        }
    }

    std::vector<Beacon> finalBeacons;

    for (const auto& rotatedScanner : rotatedScanners)
    {
        for (const auto& beacon : rotatedScanner->beacons)
        {
            if (std::find(finalBeacons.begin(), finalBeacons.end(), *beacon) == finalBeacons.end())
            {
                finalBeacons.push_back(*beacon);
//                printf("%d,%d,%d\n",beacon->vector.x, beacon->vector.y, beacon->vector.z);
            }
        }
    }

    std::cout << "19-1: " << finalBeacons.size() << std::endl;

    int maxManhattanDistance = 0;

    for (int i = 0 ; i < scannerVectors.size() - 1; i++)
    {
        const auto& leftVector = scannerVectors[i];
        for (int j = i + 1 ; j < scannerVectors.size(); j++)
        {
            const auto& rightVector = scannerVectors[j];
            auto gapVector = leftVector - rightVector;
            int manhatten = abs(gapVector.x) + abs(gapVector.y) + abs(gapVector.z);

            if (maxManhattanDistance < manhatten)
            {
                maxManhattanDistance = manhatten;
            }
        }
    }

    std::cout << "19-2: " << maxManhattanDistance << std::endl;
}