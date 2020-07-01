/*
 * TTTMCTS.h
 *
 *  Created on: Jun 13, 2015
 *      Author: geert
 */

#ifndef TTTMCTS_H_
#define TTTMCTS_H_

#include "mcts/mcts.h"
#include "TTTAction.h"
#include "TTTStrategy.h"
#include "Board.h"

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
