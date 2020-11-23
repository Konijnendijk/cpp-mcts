#include "mcts/mcts.hpp"

#ifndef CPP_MCTS_BOARD_HPP
#define CPP_MCTS_BOARD_HPP

enum class Player { CROSS,
    CIRCLE,
    NONE };

/**
 * TicTacToe Board implementation
 */
class Board : public State {
    std::vector<Player> board = std::vector<Player>(9);
    Player current = Player::CROSS;
    int turns = 0;

public:
    Board();

    /**
     * @return The player occupying the given position on the Board
     */
    Player position(int x, int y) const;

    /**
     * Play the given move for the current Player.
     */
    void play(int x, int y);

    /**
     * @return Player::CROSS or Player::CIRCLE in case of a win, Player::NONE in case of a draw or when the game is not
     * finished yet.
     */
    Player won() const;

    /**
     * @return The number of times play(x,y) has been called
     */
    int getTurns() const;

    /**
     * @return The Player allowed to make a move
     */
    Player getCurrentPlayer() const;

    /**
     * @return A single character representation of a Player
     */
    static char playerToChar(Player p)
    {
        switch (p) {
        case Player::CROSS:
            return 'x';
        case Player::CIRCLE:
            return 'o';
        default:
            return '-';
        }
    }

    void print(std::ostream& strm) override;
};

#endif // CPP_MCTS_BOARD_HPP
