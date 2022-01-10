#include <bits/stdc++.h>

int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    uint32_t previous_depth = UINT_MAX;
    uint32_t new_depth = 0;

    uint32_t count = 0;

    while(!std::cin.eof()) {
        std::cin >> new_depth;

        if (previous_depth < new_depth) {
            count++;
        }

        previous_depth = new_depth;
    }

    std::cout << count << std::endl;

    return 0;
}


