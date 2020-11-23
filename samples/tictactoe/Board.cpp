
#include "Board.hpp"
#include <cstring>
#include <iostream>

#define WIN_CHECK(a, b, c)                                                            \
    {                                                                                 \
        if (board[a] == board[b] && board[b] == board[c] && board[a] != Player::NONE) \
            return board[a];                                                          \
    }

Board::Board()
{
    std::fill(board.begin(), board.end(), Player::NONE);
}

void Board::play(int x, int y)
{
    board[y * 3 + x] = this->current;
    current = current == Player::CROSS ? Player::CIRCLE : Player::CROSS;
    turns += 1;
}

Player Board::position(int x, int y) const { return board[y * 3 + x]; }

Player Board::won() const
{
    for (int i = 0; i < 9; i += 3)
        WIN_CHECK(i, i + 1, i + 2);
    for (int i = 0; i < 3; i++)
        WIN_CHECK(i, i + 3, i + 6);
    WIN_CHECK(0, 4, 8);
    WIN_CHECK(2, 4, 6);
    return Player::NONE;
}

int Board::getTurns() const { return turns; }

Player Board::getCurrentPlayer() const { return current; }

void Board::print(std::ostream& strm)
{
    for (int i = 0; i < 9; i++) {
        if (i % 3 == 0 && i != 0)
            strm << "\\n";
        strm << Board::playerToChar(this->board[i]);
    }
}
