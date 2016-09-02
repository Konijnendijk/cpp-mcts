/*
 * TTTMCTS.cpp
 *
 *  Created on: Jun 13, 2015
 *      Author: geert
 */

#include "TTTMCTSPlayer.h"

class TTTBackpropagation : public Backpropagation<Board> {
	Player player;

public:
	TTTBackpropagation(Player player) : player(player) {}

	float updateScore(Board* state, float backpropScore) {
		return state->getCurrentPlayer() == player ? backpropScore : 1-backpropScore;
	}

	~TTTBackpropagation() override {}
};

class TTTTerminationCheck : public TerminationCheck<Board>{
	/**
	 * @return True when the game is won, or the Players draw
	 */
	bool isTerminal(Board* state) override {
		return state->won()!=NONE || state->getTurns()==9;
	}
};

class TTTScoring : public Scoring<Board>{

	Player player;

public:

	TTTScoring(Player player) : player(player) {}

	/**
	 * @return The score the given Board is assigned
	 */
	float score(Board* state) override {
		if (state->won()==player)
			return 0;
		else if (state->won()!=NONE)
			return 1;
		else
			return 0.75;
	}

	~TTTScoring() override {}
};

/**
 *
 * @param n
 * @return The Node's average score in case of the MCTS player, 1-the Node's average score in case of its opponent
 */
float selectionScore(Board* state, TTTAction* action, float score){
	if (state->getCurrentPlayer()==CIRCLE)
		return 1-score;
	else
		return score;
}

TTTMCTSPlayer::TTTMCTSPlayer(Board* board) : board(board), mcts() {}

TTTAction* TTTMCTSPlayer::calculateAction(){
	mcts = createMCTS();
	TTTAction* a=mcts->calculateAction();
	delete mcts;
	return a;
}

TTTMCTS* TTTMCTSPlayer::createMCTS(){
	return new TTTMCTS(new Board(*board), new TTTBackpropagation(board->getCurrentPlayer()), new TTTTerminationCheck(), new TTTScoring(board->getCurrentPlayer()));
}

TTTMCTSPlayer::~TTTMCTSPlayer() {}

