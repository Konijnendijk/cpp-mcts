/*
 * TTTMCTS.cpp
 *
 *  Created on: Jun 13, 2015
 *      Author: geert
 */

#include "TTTMCTSPlayer.hpp"

class TTTBackpropagation : public Backpropagation<Board> {
    Player player;

public:
    TTTBackpropagation(Player player)
        : player(player)
    {
    }

    float updateScore(const Board& state, float backpropScore) override
    {
        // When the current player is our player, the enemy has performed its move and the score should be the inverse
        return state.getCurrentPlayer() == player ? 1 - backpropScore : backpropScore;
    }

    ~TTTBackpropagation() override {}
};

class TTTTerminationCheck : public TerminationCheck<Board> {
    /**
     * @return True when the game is won, or the Players draw
     */
    bool isTerminal(const Board& state) override { return state.won() != NONE || state.getTurns() == 9; }
};

class TTTScoring : public Scoring<Board> {

    Player player;

public:
    TTTScoring(Player player)
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
        else if (state.won() != NONE)
            return 0;
        else
            return 0.75;
    }

    ~TTTScoring() override {}
};

TTTAction TTTMCTSPlayer::calculateAction(const Board& board)
{
    auto mcts = createMCTS(board);
    return mcts.calculateAction();
}

TTTMCTS TTTMCTSPlayer::createMCTS(const Board& board) const
{
    auto backpropagation = new TTTBackpropagation(board.getCurrentPlayer());
    auto terminationCheck = new TTTTerminationCheck();
    auto scoring = new TTTScoring(board.getCurrentPlayer());
    return TTTMCTS(Board(board), backpropagation, terminationCheck,
        scoring);
}

TTTMCTSPlayer::~TTTMCTSPlayer() {}
