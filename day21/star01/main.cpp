#include <vector>
#include <iostream>
#include <string>


class InputReader {
public:
    static std::vector<uint16_t> read() {
        std::ios_base::sync_with_stdio(false);
        freopen(INPUT_FILE_PATH, "rt", stdin);

        std::vector<uint16_t> points(2);
        std::string input;

        getline(std::cin, input);
        sscanf(input.c_str(), "Player 1 starting position: %hu", &points[0]);
        getline(std::cin, input);
        sscanf(input.c_str(), "Player 2 starting position: %hu", &points[1]);

        return points;
    }
};

void updateNextPos(uint16_t &pos, uint16_t &dice) {
    uint16_t diceSum = 0;
    for (int i = 0; i < 3; i++) {
        dice++;
        diceSum += dice;
    }
    pos = (pos - 1 + diceSum) % 10 + 1;
}

int main() {
    auto positions = InputReader::read();

    std::vector<uint16_t> totalPoint(2);
    uint16_t dice = 0;
    int tryCount = 1;

    while (true) {
        auto pivotIndex = (tryCount + 1) % 2;
        auto &pivotPos = positions[pivotIndex];

        updateNextPos(pivotPos, dice);

        totalPoint[pivotIndex] += pivotPos;

        if (totalPoint[pivotIndex] >= 1000) {
            break;
        }

        tryCount++;
    }

    std::cout << "21-1: " << dice * std::min(totalPoint[0], totalPoint[1]) << std::endl;
}