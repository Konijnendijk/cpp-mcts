#include "TTTStrategy.hpp"

TTTExpansionStrategy::TTTExpansionStrategy(Board* state)
    : ExpansionStrategy<Board, TTTAction>(state)
{
    searchNextPossibleMove(currentX, currentY);
}

TTTAction TTTExpansionStrategy::generateNext()
{
    int oldX = currentX;
    int oldY = currentY;

    currentY++;
    searchNextPossibleMove(currentX, currentY);
    return TTTAction(oldX, oldY);
}

void TTTExpansionStrategy::searchNextPossibleMove(int& x, int& y) const
{
    if (y == 3) {
        x++;
        y = 0;
    }

    while (x < 3) {
        while (y < 3) {
            if (state->position(x, y) == Player::NONE)
                return;
            y++;
        }
        y = 0;
        x++;
    }

    x = -1;
    y = -1;
}

bool TTTExpansionStrategy::canGenerateNext() const
{
    return currentX != -1 && currentY != -1;
}

TTTPlayoutStrategy::TTTPlayoutStrategy(Board* state)
    : PlayoutStrategy<Board, TTTAction>(state)
{
}

void TTTPlayoutStrategy::generateRandom(TTTAction& action)
{
    int x = distribution(generator);
    int y = distribution(generator);

    // search the Board until an empty square is found
    while (state->position(x, y) != Player::NONE) {
        x = distribution(generator);
        y = distribution(generator);
    }
    action.setX(x);
    action.setY(y);
}

TTTPlayoutStrategy::TTTPlayoutStrategy(const TTTPlayoutStrategy& other)
    : PlayoutStrategy<Board, TTTAction>(other.state)
{
}

TTTPlayoutStrategy::~TTTPlayoutStrategy() = default;
