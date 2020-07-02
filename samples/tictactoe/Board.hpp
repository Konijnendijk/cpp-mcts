#include "mcts/mcts.hpp"

#ifndef CPP_MCTS_BOARD_HPP
#define CPP_MCTS_BOARD_HPP

enum Player { CROSS,
    CIRCLE,
    NONE };

/**
 * TicTacToe Board implementation
 */
class Board : public State {
    Player* board;
    Player current;
    int turns;

public:
    Board();
    Board(const Board& board);

    /**
     * @return The player occupying the given position on the Board
     */
    Player position(int x, int y);

    /**
     * Play the given move for the current Player.
     */
    void play(int x, int y);

    /**
     * @return Player::CROSS or Player::CIRCLE in case of a win, Player::NONE in case of a draw or when the game is not
     * finished yet.
     */
    Player won();

    /**
     * @return The number of times play(x,y) has been called
     */
    int getTurns();

    /**
     * @return The Player allowed to make a move
     */
    Player getCurrentPlayer();

    /**
     * @return A single character representation of a Player
     */
    static char playerToChar(Player p)
    {
        switch (p) {
        case NONE:
            return '-';
        case CROSS:
            return 'x';
        case CIRCLE:
            return 'o';
        }
    }

    void print(std::ostream& strm) override;

    Board& operator=(const Board& other);
    ~Board();
};

#endif // CPP_MCTS_BOARD_HPP
