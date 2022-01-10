#include <bits/stdc++.h>

int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    int32_t depth = 0;
    int32_t horizontal = 0;

    std::string direction;
    uint32_t direction_unit = 0;
    uint32_t aim = 0;

    while(!std::cin.eof())
    {
        std::cin >> direction >> direction_unit;

        if ("forward" == direction)
        {
            horizontal += direction_unit;
            depth += aim * direction_unit;
        }
        else if ("down" == direction)
        {
            aim += direction_unit;
        }
        else if ("up" == direction)
        {
            aim -= direction_unit;
        }
    }

    std::cout << "Dive 02:" << horizontal * depth << std::endl;

    return 0;
}


