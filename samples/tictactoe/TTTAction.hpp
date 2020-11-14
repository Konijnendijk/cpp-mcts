#ifndef CPP_MCTS_TTTACTION_HPP
#define CPP_MCTS_TTTACTION_HPP

#include "Board.hpp"

/**
 * Represents an Action that can be executed on a Board.
 */
class TTTAction : public Action<Board> {
    int x, y;

public:
    TTTAction()
        : x(-1)
        , y(-1)
    {
    }

    TTTAction(int x, int y)
        : x(x)
        , y(y)
    {
    }

    TTTAction(const TTTAction& a)
        : TTTAction(a.x, a.y)
    {
    }

    /**
     * Plays the given move for the current player of the Board.
     */
    void execute(Board& data) override { data.play(x, y); }

    int getX() { return x; }
    void setX(int x) { this->x = x; }

    int getY() { return y; }
    void setY(int y) { this->y = y; }

    void print(std::ostream& strm) override { strm << "Place at (" << x << "," << y << ")"; }

    bool operator==(const TTTAction& a) { return a.x == x && a.y == y; }

    bool operator!=(const TTTAction& a) { return !operator==(a); }

    ~TTTAction() {}
};

#endif // CPP_MCTS_TTTACTION_HPP
