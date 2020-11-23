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

using TTTMCTS = MCTS<Board, TTTAction, TTTExpansionStrategy, TTTPlayoutStrategy>;

class TTTMCTSPlayer {
public:
    /**
     * Will play one move on the board passed through the constructor.
     */
    static TTTAction calculateAction(const Board& board);

private:
    /**
     * Creates a new MCTS instance.
     */
    static TTTMCTS createMCTS(const Board& board);
};

class TTTBackpropagation : public Backpropagation<Board> {
    Player player;

public:
    explicit TTTBackpropagation(Player player)
        : player(player)
    {
    }

    float updateScore(const Board& state, float backpropScore) override
    {
        // When the current player is our player, the enemy has performed its move and the score should be the inverse
        return state.getCurrentPlayer() == player ? 1 - backpropScore : backpropScore;
    }
};

class TTTTerminationCheck : public TerminationCheck<Board> {
    /**
     * @return True when the game is won, or the Players draw
     */
    bool isTerminal(const Board& state) override { return state.won() != Player::NONE || state.getTurns() == 9; }
};

class TTTScoring : public Scoring<Board> {

    Player player;

public:
    explicit TTTScoring(Player player)
        : player(player)
    {
    }

    /**
     * @return The score the given Board is assigned
     */
    float score(const Board& state) override
    {
        if (state.won() == player)
            return 1;
        else if (state.won() != Player::NONE)
            return 0;
        else
            return 0.75;
    }
};

#endif // CPP_MCTS_TTTMCTS_HPP
