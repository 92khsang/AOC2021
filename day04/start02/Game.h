//
// Created by hayes on 1/6/22.
//

#ifndef ADVENTOFCODE2021_GAME_H
#define ADVENTOFCODE2021_GAME_H

#include "Board.h"
#include <vector>

class Game
{
public:
    void addBoard(const Board& board);
    void mark(uint32_t number);
    bool isGameOver() const;
    int32_t getResult();


private:
    void updateTargetPoint(Board& board, Point& point);
    void updateNeighborPoints(Board& board, Point& point);
    bool isBingo(Point& point) const;

private:
    std::vector<Board> boards;
    bool over = false;
    int32_t lastBingoBoardIndex = -1;
    int32_t lastCalled = -1;
};

#endif //ADVENTOFCODE2021_GAME_H
