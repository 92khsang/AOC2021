#include <bits/stdc++.h>
#include <vector>

size_t countTheNumberOfOneFromFirstChar(const std::vector<std::string>& bits)
{
    size_t count = 0;
    for (const auto& bit : bits)
    {
        if ('1' == bit[0])
        {
            count++;
        }
    }
    return count;

}

char oxygenCompare(size_t num_of_1, size_t total_bits)
{
    return num_of_1 >= (total_bits - num_of_1) ? '1' : '0';
}

char co2Compare(size_t num_of_1, size_t total_bits)
{
    return num_of_1 < (total_bits - num_of_1) ? '1' : '0';
}

std::string makeRatingStr(const std::vector<std::string>& bits, char (*compare)(size_t, size_t))
{

    if (bits.size() == 1)
    {
        return bits[0];
    }

    const auto num_of_1 = countTheNumberOfOneFromFirstChar(bits);

    std::vector<std::string> new_bits;
    const char targetBinary = compare(num_of_1, bits.size());

    for (const auto& bit : bits)
    {
        if (targetBinary == bit[0])
        {
            std::string new_bit = bit;
            new_bit.erase(0, 1);
            new_bits.push_back(new_bit);
        }
    }

    return targetBinary + makeRatingStr(new_bits, compare);
}

int main()
{

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    std::string bit;
    std::vector<std::string> bits;

    while(!std::cin.eof())
    {
        std::cin >> bit;
        bits.push_back(bit);
    }

    std::string oxygen_str = makeRatingStr(bits, oxygenCompare);
    std::string co2_str = makeRatingStr(bits, co2Compare);

    auto oxygen = std::stoull(oxygen_str, 0, 2);
    auto co2 = std::stoull(co2_str, 0, 2);

    std::cout << "Day 03-2:" << oxygen * co2 << std::endl;

    return 0;
}


