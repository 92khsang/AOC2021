#include <bits/stdc++.h>
#include <vector>
#include <array>
#include <iostream>


struct TargetArea
{
    std::pair<int32_t, int32_t> xRange;
    std::pair<int32_t, int32_t> yRange;
};

class TargetAreaMaker
{
public:
    static TargetArea make()
    {
        TargetArea area;

        std::string input;
        getline(std::cin, input);

        sscanf(input.c_str(), "target area: x=%d..%d, y=%d..%d", &area.xRange.first, &area.xRange.second, &area.yRange.first, &area.yRange.second);

        return area;
    }
};

//class MinStepCalculator
//{
//public:
//    static int32_t getMinStep(int32_t dest)
//    {
//        auto stepMultipliedByTwo = static_cast<int32_t>(std::sqrt(dest * 8 - 3) + 1);
//        return (stepMultipliedByTwo / 2);
//    }
//};

 struct ReachableMap
 {
 public:
     using step = int32_t;

     std::map<step, std::vector<std::pair<int32_t, int32_t>>> velocities;
 };

 class ReachableMapMaker
 {
 public:
     static ReachableMap make(const int32_t start, const int32_t end)
     {
         ReachableMap newMap;

         addOneStepReachableVelocities(newMap.velocities[1], start, end);

         int32_t step = 2;
         auto maxSpeed = getMaxSpeedToReachEnd(end, step);

         while (maxSpeed - step + 1 >= 0)
         {
             auto maxReachableDest = step * maxSpeed - step * (step -1) / 2;
             auto speed = maxSpeed;
             while (maxReachableDest >= start)
             {
                 int32_t startVelocity = speed - step + 1;
                 int32_t endVelocity = speed;

                 newMap.velocities[step].emplace_back(startVelocity, endVelocity);
                 speed--;
                 maxReachableDest -= step;
             }

             step++;
             maxSpeed = getMaxSpeedToReachEnd(end, step);
         }

         return newMap;
     }

 private:
    static void addOneStepReachableVelocities(std::vector<std::pair<int32_t, int32_t>>& velocities, const int32_t start, const int32_t end)
    {
        for (int i = end; i >= start; i--)
        {
            velocities.emplace_back(i, i);
        }
    }

    static int32_t getMaxSpeedToReachEnd(const int32_t end, const int32_t step)
    {
        return static_cast<int32_t>(floor(static_cast<float>(end) / step + static_cast<float>(step - 1) / 2));
    }
 };

 class InitialVectorCounter
 {
 public:
     static size_t count(const ReachableMap& xReachableMap, const ReachableMap& yReachableMap)
     {
         std::set<std::pair<int32_t, int32_t>> initialVectors;
         auto updatedXReachableMap = removeDuplicatedVelocities(xReachableMap);
         auto updatedYReachableMap = updateVelocitiesThrownUpwards(yReachableMap);

         for (const auto& xVelocity : updatedXReachableMap.velocities[1])
         {
             for (const auto& yVelocity : updatedYReachableMap.velocities[1])
             {
                 initialVectors.emplace(xVelocity.second, -yVelocity.first);
             }
         }

         for (const auto& item : updatedXReachableMap.velocities)
         {
             auto xStep = item.first;
             auto xVelocities = item.second;

             if (xStep == 1)
             {
                 continue;
             }

             for (const auto& xVelocity : xVelocities)
             {
                 auto endXVelocity = xVelocity.first;
                 if (1 == endXVelocity)
                 {
                     for (auto it = updatedYReachableMap.velocities.find(xStep); it != updatedYReachableMap.velocities.end(); it++)
                     {
                         for (const auto& yVelocity : it->second)
                         {
                             initialVectors.emplace(xVelocity.second, -yVelocity.first);
                         }
                     }
                 }
                 else
                 {
                     for (const auto& yVelocity : updatedYReachableMap.velocities[xStep])
                     {
                         initialVectors.emplace(xVelocity.second, -yVelocity.first);
                     }
                 }
             }
         }

         return initialVectors.size();
     }

 private:
     static ReachableMap removeDuplicatedVelocities(const ReachableMap& reachableMap)
     {
         ReachableMap newReachableMap;

         for (const auto& item : reachableMap.velocities)
         {
             auto step = item.first;
             auto velocities = item.second;

             for (const auto& velocity : velocities)
             {
                 auto startVelocity = velocity.first;
                 if (startVelocity != 0)
                 {
                     newReachableMap.velocities[step].push_back(velocity);
                 }
             }
         }

         return newReachableMap;
     }

     static ReachableMap updateVelocitiesThrownUpwards(const ReachableMap& reachableMap)
     {
         ReachableMap newReachableMap;

         for (const auto& item : reachableMap.velocities)
         {
             auto step = item.first;
             auto velocities = item.second;

             for (const auto& velocity : velocities)
             {
                 newReachableMap.velocities[step].push_back(velocity);

                 auto startVelocity = velocity.first;
                 if (startVelocity > 1)
                 {
                    auto upwardVelocity = startVelocity - 1;
                    auto increasedStep = step + (upwardVelocity * 2) + 1;
                    newReachableMap.velocities[increasedStep].emplace_back(-upwardVelocity, velocity.second);
                 }
             }
         }

         return newReachableMap;
     }

 };

int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    auto area = TargetAreaMaker::make();
//    auto xMinStep = MinStepCalculator::getMinStep(area.xRange.first);

    auto deepestY = area.yRange.first;
    auto maxYvelocity = -deepestY - 1;
    std::cout << "Day 16-1: " << maxYvelocity * (maxYvelocity + 1) / 2  << " " << std::endl;


    auto xReachableMap = ReachableMapMaker::make(area.xRange.first, area.xRange.second);
    auto yReachableMap = ReachableMapMaker::make(-area.yRange.second, -area.yRange.first);
    std::cout << "Day 16-2: " << InitialVectorCounter::count(xReachableMap, yReachableMap) << " " << std::endl;

    return 0;
}


