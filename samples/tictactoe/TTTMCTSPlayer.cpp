/*
 * TTTMCTS.cpp
 *
 *  Created on: Jun 13, 2015
 *      Author: geert
 */

#include "TTTMCTSPlayer.hpp"

TTTAction TTTMCTSPlayer::calculateAction(const Board& board)
{
    auto mcts = createMCTS(board);
    return mcts.calculateAction();
}

TTTMCTS TTTMCTSPlayer::createMCTS(const Board& board)
{
    auto backpropagation = new TTTBackpropagation(board.getCurrentPlayer());
    auto terminationCheck = new TTTTerminationCheck();
    auto scoring = new TTTScoring(board.getCurrentPlayer());
    return TTTMCTS(Board(board), backpropagation, terminationCheck,
        scoring);
}
