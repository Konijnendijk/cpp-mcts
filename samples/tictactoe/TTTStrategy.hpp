#ifndef CPP_MCTS_TTTACTIONGENERATOR_HPP
#define CPP_MCTS_TTTACTIONGENERATOR_HPP

#include "TTTAction.hpp"

/**
 * Generates possible TTTActions
 */
class TTTExpansionStrategy : public ExpansionStrategy<Board, TTTAction> {
    int currentX, currentY;

public:
    explicit TTTExpansionStrategy(Board* state);

    TTTAction generateNext() override;

    bool canGenerateNext() override;

private:
    /**
     * Increment x and y until an empty square is found
     */
    void searchNextPossibleMove(int& x, int& y);
};

class TTTPlayoutStrategy : public PlayoutStrategy<Board, TTTAction> {

public:
    explicit TTTPlayoutStrategy(Board* state);

    void generateRandom(TTTAction* action) override;
};

#endif // CPP_MCTS_TTTACTIONGENERATOR_HPP
