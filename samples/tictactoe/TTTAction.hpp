#ifndef CPP_MCTS_TTTACTION_HPP
#define CPP_MCTS_TTTACTION_HPP

#include "Board.hpp"

/**
 * Represents an Action that can be executed on a Board.
 */
class TTTAction : public Action<Board> {
    int x = -1;
    int y = -1;

public:
    TTTAction() = default;

    TTTAction(int x, int y)
        : x(x)
        , y(y)
    {
    }

    /**
     * Plays the given move for the current player of the Board.
     */
    void execute(Board& data) override { data.play(x, y); }

    int getX() const { return x; }
    void setX(int newX) { this->x = newX; }

    int getY() const { return y; }
    void setY(int newY) { this->y = newY; }

    void print(std::ostream& strm) override { strm << "Place at (" << x << "," << y << ")"; }

    bool operator==(const TTTAction& a) const { return a.x == x && a.y == y; }

    bool operator!=(const TTTAction& a) const { return !operator==(a); }
};

#endif // CPP_MCTS_TTTACTION_HPP
