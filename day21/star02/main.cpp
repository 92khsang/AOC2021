#include <vector>
#include <iostream>
#include <string>
#include <chrono>
#include <algorithm>
#include <unordered_map>

using WinCount = std::pair<size_t, size_t>;

struct Posibility
{
    uint16_t diceSum;
    uint16_t frequency;
};

const std::vector<Posibility> POSIBILITIES = {
        {3, 1},
        {4, 3},
        {5, 6},
        {6, 7},
        {7, 6},
        {8, 3},
        {9, 1},
};

class Player
{
public:
    Player(int16_t leftPoint, uint16_t pos) : leftPoint(leftPoint), pos(pos) {}
    Player() : leftPoint(21), pos(0) {}

    bool operator==(const Player &rhs) const {
        return leftPoint == rhs.leftPoint &&
               pos == rhs.pos;
    }

    bool operator!=(const Player &rhs) const {
        return !(rhs == *this);
    }

public:
    int16_t leftPoint = 21;
    uint16_t pos;
};

class PlaySet
{
public:
    PlaySet(const Player &thisTurnPlayer, const Player &prevTurnPlayer) :
        thisTurnPlayer(thisTurnPlayer),
        prevTurnPlayer(prevTurnPlayer) {}

    PlaySet() = default;

public:
    bool operator==(const PlaySet &rhs) const {
        return thisTurnPlayer == rhs.thisTurnPlayer &&
               prevTurnPlayer == rhs.prevTurnPlayer;
    }

    bool operator!=(const PlaySet &rhs) const {
        return !(rhs == *this);
    }

public:
    Player thisTurnPlayer;
    Player prevTurnPlayer;
};


struct play_hash
{
    std::size_t operator() (const PlaySet &set) const {
        size_t hash_value = (static_cast<size_t>(set.thisTurnPlayer.leftPoint) << 48) +
                            (static_cast<size_t>(set.thisTurnPlayer.pos) << 32) +
                            (static_cast<size_t>(set.prevTurnPlayer.leftPoint) << 16) +
                            (static_cast<size_t>(set.prevTurnPlayer.pos));
        return hash_value;
    }
};

static std::unordered_map<PlaySet, WinCount, play_hash> history;

class InputReader
{

public:
    static std::vector<Player> read()
    {
        std::ios_base::sync_with_stdio(false);
        freopen(INPUT_FILE_PATH, "rt", stdin);
        std::vector<Player> player(2);

        std::string input;
        getline(std::cin, input);
        sscanf(input.c_str(), "Player 1 starting position: %hu", &player[0].pos);
        getline(std::cin, input);
        sscanf(input.c_str(), "Player 2 starting position: %hu", &player[1].pos);

        return player;
    }
};

WinCount play(Player thisTurnPlayer, Player prevTurnPlayer) {

    if (prevTurnPlayer.leftPoint <= 0)
    {
        return { 0, 1 };
    }

    PlaySet playSet(thisTurnPlayer, prevTurnPlayer);
    const auto& foundIt = history.find(playSet);
    if (foundIt != history.end()) {
        return foundIt->second;
    }

    WinCount winCount;
    auto& winCountOfThisTurnPlayer = winCount.first;
    auto& winCountOfPrevTurnPlayer = winCount.second;

    for (const auto& posibility : POSIBILITIES)
    {
        uint16_t nextPos = (thisTurnPlayer.pos - 1 + posibility.diceSum) % 10 + 1;
        Player playingPlayer{ static_cast<int16_t>(thisTurnPlayer.leftPoint - nextPos), nextPos };

        auto nextWinCount = play(prevTurnPlayer, playingPlayer);

        const auto& winCountOfThisTurnPlayerAfterEndOfGame = nextWinCount.second;
        const auto& winCountOfPrevTurnPlayerAfterEndOfGame = nextWinCount.first;

        winCountOfThisTurnPlayer += winCountOfThisTurnPlayerAfterEndOfGame * posibility.frequency;
        winCountOfPrevTurnPlayer += winCountOfPrevTurnPlayerAfterEndOfGame * posibility.frequency;
    }

    history.emplace(playSet, winCount);

    return winCount;
}

int main()
{
    auto players = InputReader::read();


    std::chrono::system_clock::time_point StartTime = std::chrono::system_clock::now();
    auto winCount = play(players[0], players[1]);
    std::chrono::system_clock::time_point EndTime = std::chrono::system_clock::now();

    std::chrono::milliseconds mill  = std::chrono::duration_cast<std::chrono::milliseconds>(EndTime - StartTime);

    std::cout << "21-2: " << std::max(winCount.first, winCount.second) << std::endl;
    std::cout << "Playing Time : " << mill.count() << " ms" << std::endl;
}