#include <bits/stdc++.h>
#include <vector>

#include "Board.h"
#include "Game.h"

int main(){

    std::ios_base::sync_with_stdio(false);
    freopen(INPUT_FILE_PATH, "rt", stdin);

    std::string line;
    std::vector<uint32_t> selects;

    std::getline(std::cin, line);
    std::stringstream selectStream(line);

    for (uint32_t i; selectStream >> i;) {
        selects.push_back(i);
        if (selectStream.peek() == ',')
            selectStream.ignore();
    }


    Game game;

    std::vector<Board> boards;

    size_t rows = 1;
    size_t cols = 1;

    while(!std::cin.eof())
    {
        std::getline(std::cin, line);

        if (line.empty())
        {
            boards.emplace_back();

            rows = 1;
            cols = 1;
            continue;
        }


        auto& board = boards.back();

        std::stringstream valueStream(line);

        for (uint32_t value; valueStream >> value;) {
            board.addNewPoint(cols++, rows, value);

            if (valueStream.peek() == ' ')
                valueStream.ignore();
        }
        rows++;
        cols = 1;
    }

    for (auto& board : boards)
    {
        game.addBoard(board);
    }

    for (const auto select : selects)
    {
        game.mark(select);

        if (game.isGameOver())
        {
            break;
        }
    }

    std::cout << "Day 04-1:" << game.getResult() << std::endl;

    return 0;
}


