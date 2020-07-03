/*
 * TTTMCTS.h
 *
 *  Created on: Jun 13, 2015
 *      Author: geert
 */

#ifndef CPP_MCTS_TTTMCTS_HPP
#define CPP_MCTS_TTTMCTS_HPP

#include "Board.hpp"
#include "TTTAction.hpp"
#include "TTTStrategy.hpp"
#include "mcts/mcts.hpp"

typedef MCTS<Board, TTTAction, TTTExpansionStrategy, TTTPlayoutStrategy> TTTMCTS;

class TTTMCTSPlayer {
    Board* board;

public:
    TTTMCTSPlayer(Board* board);

    /**
     * Will play one move on the board passed through the constructor.
     */
    TTTAction calculateAction();

    ~TTTMCTSPlayer();

private:
    /**
     * Creates a new MCTS instance.
     */
    TTTMCTS createMCTS();
};

#endif // CPP_MCTS_TTTMCTS_HPP
