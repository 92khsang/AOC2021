#include <bits/stdc++.h>
#include <vector>

inline void updateBitCount(const std::string& bit, std::vector<size_t>& num_of_1)
{
    size_t index = 0;
    for (const char binary : bit)
    {
        if ('1' == binary)
        {
            num_of_1[index]++;
        }
        index++;
    }
}

inline std::pair<uint64_t, uint64_t> calculateGammaAndEpsilon(const std::vector<size_t>& num_of_1, size_t bit_count)
{
    uint64_t gamma = 0;
    uint64_t epsilon = 0;
    uint32_t digit = 1;

    for (int i = num_of_1.size() - 1; i >= 0; i--)
    {
        if (num_of_1[i] > (bit_count - num_of_1[i]))
        {
            gamma += digit;
        }
        else
        {
            epsilon += digit;
        }

        digit *= 2;
    }

    return {gamma, epsilon};
}


int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    std::string bit;
    size_t digit_count = 0;
    size_t bit_count = 0;

    std::cin >> bit;
    bit_count++;
    digit_count = bit.length();

    std::vector<size_t> num_of_1(digit_count, 0);

    updateBitCount(bit, num_of_1);

    while(!std::cin.eof())
    {
        std::cin >> bit;
        updateBitCount(bit, num_of_1);
        bit_count++;
    }

    const auto result = calculateGammaAndEpsilon(num_of_1, bit_count);

    std::cout << "Day 03-1:" << result.first * result.second << std::endl;

    return 0;
}


