/*
 * TTTMCTS.h
 *
 *  Created on: Jun 13, 2015
 *      Author: geert
 */

#ifndef TTTMCTS_H_
#define TTTMCTS_H_

#include "mcts/mcts.hpp"
#include "TTTAction.hpp"
#include "TTTStrategy.hpp"
#include "Board.hpp"

typedef MCTS<Board, TTTAction, TTTExpansionStrategy, TTTPlayoutStrategy> TTTMCTS;

class TTTMCTSPlayer {
	Board* board;
	TTTMCTS* mcts;

public:
	TTTMCTSPlayer(Board* board);

	/**
	 * Will play one move on the board passed through the constructor.
	 */
	TTTAction* calculateAction();

	~TTTMCTSPlayer();

private:
	/**
	 * Creates a new MCTS instance.
	 */
	TTTMCTS* createMCTS();
};

#endif /* TTTMCTS_H_ */
