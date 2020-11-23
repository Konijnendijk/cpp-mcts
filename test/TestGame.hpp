
/** @file test_game.h
 * @brief Implements a simple game for testing purposes. The goal of the game is to find a sequence of numbers (like
 * solving a combination lock).
 *
 * The game is single-player and has the following rules:
 *  - Each turn the player must choose a number between 0 to n
 *  - The same number can be chosen multiple times
 *  - There is a set number of m turns
 *  - After m turns, the algorithm is rewarded 1/m points for each correct number
 */

#ifndef CPP_MCTS_TESTGAME_HPP
#define CPP_MCTS_TESTGAME_HPP

#include <chrono>
#include <random>
#include <vector>

#include "mcts/mcts.hpp"

/**
 * @brief The game state.
 *
 * Stores the numbers chosen thus far and the parameters of the current game
 * (total number of turns and maximum number that can be chosen each turn).
 */
class TestGameState : public State {
private:
    // the number of times a number has to be chosen
    uint numTurns;
    // the maximum number that can be chosen each turn
    uint maxChoice;

    // The chosen numbers
    std::vector<uint> choices;

public:
    TestGameState(uint numTurns, uint maxChoice)
        : numTurns(numTurns)
        , maxChoice(maxChoice)
    {
    }

    /**
     * @brief Add a chosen number to the state, effectively advancing the game by one turn.
     *
     * @param choice the choice to add
     */
    void addChoice(uint choice) { choices.push_back(choice); }

    /**
     * @brief Get the total number of turns in the game.
     *
     * Equal to the number of numbers to pick.
     *
     * @return the total number of turns
     */
    uint getNumTurns() const { return numTurns; }

    /**
     * @brief Get the maximum number that can be chosen in the game.
     *
     * @return the maximum number, inclusive
     */
    uint getMaxChoice() const { return maxChoice; }

    /**
     * @brief Get the numbers chosen thus far.
     *
     * @return the sequence of chosen numbers
     */
    const std::vector<uint>& getChoices() const { return choices; }
};

/**
 * @brief A number that can be chosen.
 *
 * Models the player choosing a certain number this turn.
 */
class TestGameAction : public Action<TestGameState> {
private:
    uint choice = 0;

public:
    TestGameAction() = default;

    explicit TestGameAction(uint choice)
        : choice(choice)
    {
    }

    void execute(TestGameState& state) override { state.addChoice(choice); }

    void setChoice(uint newChoice) { this->choice = newChoice; }
};

/**
 * @brief Generates child states from the smallest possible choices to the largest.
 */
class TestGameExpansionStrategy : public ExpansionStrategy<TestGameState, TestGameAction> {
    using ExpansionStrategy<TestGameState, TestGameAction>::ExpansionStrategy;

private:
    uint currentChoice = 0;

public:
    TestGameAction generateNext() override { return TestGameAction(currentChoice++); }

    bool canGenerateNext() const override { return currentChoice <= state->getMaxChoice(); }
};

/**
 * @brief generate random numbers to choose given a state.
 */
class TestGamePlayoutStrategy : public PlayoutStrategy<TestGameState, TestGameAction> {
private:
    std::mt19937 generator = std::mt19937(42);
    std::uniform_int_distribution<uint> distribution;

public:
    explicit TestGamePlayoutStrategy(TestGameState* state)
        : PlayoutStrategy(state)
        , distribution(0, state->getMaxChoice())
    {
    }

    void generateRandom(TestGameAction& action) override { action.setChoice(distribution(generator)); }
};

/**
 * @brief Reward 1/m points for each correct number in the sequence, where m is the length of the sequence.
 */
class TestGameScoring : public Scoring<TestGameState> {
private:
    std::vector<uint> correctNumbers;

public:
    explicit TestGameScoring(std::vector<uint> correctNumbers)
        : correctNumbers(std::move(correctNumbers))
    {
    }

    /**
     * Score the chosen numbers in the sequence.
     *
     * Each correct number is worth 1/m points, where m is the length of the sequence.
     *
     * @param state the state to score
     * @return the score
     */
    float score(const TestGameState& state) override
    {
        const auto& choices = state.getChoices();
        uint difference = 0;
        for (int i = 0; i < choices.size(); i++) {
            if (choices[i] != correctNumbers[i]) {
                difference++;
            }
        }
        return (float)(choices.size() - difference) / (float)choices.size();
    }
};

/**
 * @brief Single player back propagation, returns the given score without modification.
 */
class TestGameBackPropagation : public Backpropagation<TestGameState> {
public:
    float updateScore(const TestGameState& state, float backpropScore) override { return backpropScore; }
};

/**
 * @brief Checks if all numbers in the sequence have been chosen.
 */
class TestGameTerminationCheck : public TerminationCheck<TestGameState> {
public:
    bool isTerminal(const TestGameState& state) override { return state.getChoices().size() == state.getNumTurns(); }
};

/**
 * @brief Convenience type definition for an MCTS agent configured with all the classes from this header.
 */
using TestGameMCTS = MCTS<TestGameState, TestGameAction, TestGameExpansionStrategy, TestGamePlayoutStrategy>;

#endif // CPP_MCTS_TESTGAME_HPP
