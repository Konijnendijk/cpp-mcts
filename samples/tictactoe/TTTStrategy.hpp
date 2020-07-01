#ifndef CPP_MCTS_TTTACTIONGENERATOR_HPP
#define CPP_MCTS_TTTACTIONGENERATOR_HPP

#include "TTTAction.hpp"

/**
 * Generates possible TTTActions
 */
class TTTExpansionStrategy : public ExpansionStrategy<Board, TTTAction>{
	int currentX,currentY;

public:
	TTTExpansionStrategy(Board* state);

	TTTAction* generateNext();

	bool canGenerateNext(){
		return currentX!=-1 && currentY!=-1;
	};

	~TTTExpansionStrategy() override {}

private:
	/**
	 * Increment x and y until an empty square is found
	 */
	void searchNextPossibleMove(int& x, int&y);

};

class TTTPlayoutStrategy : public PlayoutStrategy<Board, TTTAction>{

public:
	TTTPlayoutStrategy(Board* state);

	void generateRandom(TTTAction* action) override;

	~TTTPlayoutStrategy() override {}

};


#endif //CPP_MCTS_TTTACTIONGENERATOR_HPP
