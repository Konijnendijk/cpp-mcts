#include "TTTStrategy.hpp"

TTTExpansionStrategy::TTTExpansionStrategy(Board* state) :
		ExpansionStrategy<Board, TTTAction>(state), currentX(0), currentY(0) {
	searchNextPossibleMove(currentX, currentY);
}

TTTAction* TTTExpansionStrategy::generateNext() {
	TTTAction* a = new TTTAction(currentX, currentY);

	currentY++;
	searchNextPossibleMove(currentX, currentY);
	return a;
}

void TTTExpansionStrategy::searchNextPossibleMove(int& x, int&y) {
	if (y == 3) {
		x++;
		y = 0;
	}

	while (x < 3) {
		while (y < 3) {
			if (state->position(x, y) == NONE)
				return;
			y++;
		}
		y = 0;
		x++;
	}

	x = -1;
	y = -1;
}


TTTPlayoutStrategy::TTTPlayoutStrategy(Board* state) : PlayoutStrategy<Board, TTTAction>(state) {}

void TTTPlayoutStrategy::generateRandom(TTTAction* action) {
	int x = rand() % 3, y = rand() % 3;

	// search the Board until an empty square is found
	while (state->position(x, y) != NONE) {
		x = rand() % 3, y = rand() % 3;
	}
	action->setX(x);
	action->setY(y);

}
