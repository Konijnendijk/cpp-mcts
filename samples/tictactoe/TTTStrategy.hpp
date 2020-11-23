#ifndef CPP_MCTS_TTTACTIONGENERATOR_HPP
#define CPP_MCTS_TTTACTIONGENERATOR_HPP

#include "TTTAction.hpp"

/**
 * Generates possible TTTActions
 */
class TTTExpansionStrategy : public ExpansionStrategy<Board, TTTAction> {
    int currentX = 0;
    int currentY = 0;

public:
    explicit TTTExpansionStrategy(Board* state);

    TTTAction generateNext() override;

    bool canGenerateNext() const override;

private:
    /**
     * Increment x and y until an empty square is found
     */
    void searchNextPossibleMove(int& x, int& y) const;
};

class TTTPlayoutStrategy : public PlayoutStrategy<Board, TTTAction> {
    std::random_device device;
    std::mt19937 generator = std::mt19937(device());
    std::uniform_int_distribution<uint> distribution = std::uniform_int_distribution<uint>(0, 2);

public:
    explicit TTTPlayoutStrategy(Board* state);
    TTTPlayoutStrategy(const TTTPlayoutStrategy& other);
    ~TTTPlayoutStrategy() override;

    void generateRandom(TTTAction& action) override;
};

#endif // CPP_MCTS_TTTACTIONGENERATOR_HPP
