//
// Created by hayes on 1/6/22.
//


#include "Board.h"


const std::array<Movement, Direction::MAX_DIRECTION_COUNT> Board::movement = {
        Movement(0, -1),
        Movement(-1, 0),
        Movement(0, 1),
        Movement(1, 0)
};

void Board::addNewPoint(int32_t x, int32_t y, uint32_t value) {
    auto& point = points[y][x];

    point.location.x = x;
    point.location.y = y;
    point.value = value;

    map[value] = point.location;
}

bool Board::isOutOfBound(int32_t x, int32_t y) {
    return (x <= 0 || y <= 0 || x > BOARD_SIZE || y > BOARD_SIZE);
}

Point& Board::getPoint(int32_t x, int32_t y) {
    return points[y][x];
}

Point &Board::getPoint(uint32_t value) {
    return points[map[value].y][map[value].x];
}

bool Board::DoesPointExist(uint32_t value) const {
    return (map.find(value) != map.end());
}

bool Board::HasBingo() const {
    return bingo;
}

void Board::Bingo() {
    bingo = true;
}

//bool Point::operator==(const Point &r) const {
//    return (this->location.x == r.location.x && this->location.y == r.location.y && this->value == r.value);
//}
