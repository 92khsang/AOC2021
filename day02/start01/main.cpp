#include <bits/stdc++.h>

int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    int32_t depth = 0;
    int32_t horizontal = 0;

    std::string direction;
    uint32_t direction_unit = 0;

    while(!std::cin.eof())
    {
        std::cin >> direction >> direction_unit;

        if ("forward" == direction)
        {
             horizontal += direction_unit;
        }
        else if ("down" == direction)
        {
            depth += direction_unit;
        }
        else if ("up" == direction)
        {
            depth -= direction_unit;
            if (depth < 0)
            {
                depth = 0;
            }
        }
    }

    std::cout << "Dive 01:" << horizontal * depth << std::endl;

    return 0;
}


