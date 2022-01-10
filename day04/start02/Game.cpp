//
// Created by hayes on 1/6/22.
//

#include "Game.h"

#include <cassert>


void Game::addBoard(const Board& board)
{
    boards.push_back(board);
}

void  Game::mark(uint32_t number)
{
    if (over)
    {
        return;
    }

    size_t bingoBoardCount = 0;

    for (int i = 0 ; i < boards.size(); i++)
    {
        auto& board = boards[i];
        if (board.HasBingo())
        {
            bingoBoardCount++;
            continue;
        }

        if (!board.DoesPointExist(number))
        {
            continue;
        }

        auto& point = board.getPoint(number);

        updateTargetPoint(board, point);
        updateNeighborPoints(board, point);

        if (isBingo(point))
        {
            bingoBoardCount++;
            board.Bingo();
            lastBingoBoardIndex = i;
        }
    }

    if (bingoBoardCount == boards.size())
    {
        over = true;
        lastCalled = number;
    }
}

void Game::updateTargetPoint(Board& board, Point& point)
{

    assert(!point.marked);

    for (int direction = 0 ; direction < Direction::MAX_DIRECTION_COUNT; direction++)
    {
        int neighborPointX = point.location.x + Board::movement[direction].x;
        int neighborPointY = point.location.y + Board::movement[direction].y;

        if (Board::isOutOfBound(neighborPointX, neighborPointY))
        {
            point.updateFarthestPoint(point, direction);
        }
        else
        {
            auto& neighborPoint = board.getPoint(neighborPointX, neighborPointY);

            if (neighborPoint.marked)
            {
                point.updateFarthestPoint(neighborPoint, direction);
            }
            else
            {
                point.updateFarthestPoint(point, direction);
            }

        }
    }

    point.marked = true;
}

void Game::updateNeighborPoints(Board &board, Point &point)
{

    for (int direction = 0 ; direction < Direction::MAX_DIRECTION_COUNT; direction++)
    {
        int neighborPointX = point.location.x + Board::movement[direction].x;
        int neighborPointY = point.location.y + Board::movement[direction].y;

        if (!Board::isOutOfBound(neighborPointX, neighborPointY))
        {
            auto& neighborPoint = board.getPoint(neighborPointX, neighborPointY);

            const auto oppositeDirection = Direction::getOppositeDirection(direction);

            neighborPoint.updateFarthestPoint(point, oppositeDirection);
        }
    }
}

bool Game::isBingo(Point &point) const {
    bool bingo = false;

    for (int direction = 0; direction < (Direction::MAX_DIRECTION_COUNT / 2); direction++)
    {
        const auto oppositeDirection = Direction::getOppositeDirection(direction);

        const auto& edgeInDirection = *point.findFarthestMarkedPoint(direction);
        const auto& edgeInOppositeDirection = *point.findFarthestMarkedPoint(oppositeDirection);

        int32_t distance = std::abs(edgeInDirection.location.x - edgeInOppositeDirection.location.x) + std::abs(edgeInDirection.location.y - edgeInOppositeDirection.location.y);

        bingo = ((Board::BOARD_SIZE -1) == distance);

        if (bingo)
        {
            break;
        }
    }

    return bingo;
}

bool Game::isGameOver() const
{
    return over;
}

int32_t Game::getResult()
{
    if (!over)
    {
        return 0;
    }

    int32_t sum = 0;

    auto& board = boards[lastBingoBoardIndex];

    for (size_t i = 1 ; i <= Board::BOARD_SIZE; i++ )
    {
        for (size_t j = 1 ; j <= Board::BOARD_SIZE; j++ )
        {
            const auto& point = board.getPoint(i, j);

            if (!point.marked)
            {
                sum += point.value;
            }
        }
    }

    return sum * lastCalled;
}
