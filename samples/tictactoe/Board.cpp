
#include "Board.hpp"
#include <cstring>
#include <iostream>

#define WIN_CHECK(a, b, c)                                                    \
    {                                                                         \
        if (board[a] == board[b] && board[b] == board[c] && board[a] != NONE) \
            return board[a];                                                  \
    }

Board::Board()
    : turns(0)
{
    board = new Player[9];
    for (int i = 0; i < 9; i++)
        board[i] = NONE;
    current = CROSS;
}

Board::Board(const Board& old)
{
    board = new Player[9];
    memcpy(board, old.board, sizeof(Player) * 9);
    current = old.current;
    turns = old.turns;
}

void Board::play(int x, int y)
{
    board[y * 3 + x] = this->current;
    current = current == CROSS ? CIRCLE : CROSS;
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
    return NONE;
}

int Board::getTurns() const { return turns; }

Player Board::getCurrentPlayer() const { return current; }

Board& Board::operator=(const Board& other)
{
    if (this != &other) {
        memcpy(board, other.board, sizeof(Player) * 9);
        current = other.current;
        turns = other.turns;
    }
    return *this;
}

void Board::print(std::ostream& strm)
{
    for (int i = 0; i < 9; i++) {
        if (i % 3 == 0 && i != 0)
            strm << "\\n";
        strm << Board::playerToChar(this->board[i]);
    }
}

Board::~Board() { delete[] board; }
