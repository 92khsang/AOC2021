#include <bits/stdc++.h>
#include <vector>

int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    std::vector<uint32_t> depths(4, 0);

    uint32_t increase_count = 0;
    uint32_t index = 0;

    for (; index < depths.size() - 1 ; index++) {
        std::cin >> depths[index];
    }

    while(!std::cin.eof()) {
        std::cin >> depths[index % depths.size()];

        uint32_t new_depth = depths[index % depths.size()];
        uint32_t previous_depth = depths[(index + 1) % depths.size()];

        if (previous_depth < new_depth) {
            increase_count++;
        }

        index++;
    }

    std::cout << increase_count << std::endl;

    return 0;
}


