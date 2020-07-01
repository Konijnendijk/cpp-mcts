
/** @file test_game.h
 * @brief Implements a simple game for testing purposes. The goal of the game is to find a sequence of numbers (like solving a combination lock).
 *
 * The game is single-player and has the following rules:
 *  - Each turn the player must choose a number between 0 to n
 *  - The same number can be chosen multiple times
 *  - There is a set number of m turns
 *  - After m turns, the algorithm is rewarded 1/m points for each correct number
 */

#ifndef CPP_MCTS_TESTGAME_H
#define CPP_MCTS_TESTGAME_H

#include <random>
#include <vector>
#include <chrono>

#include "mcts.h"

class TestGameState : public State {
private:
    // the number of times a number has to be chosen
    uint numTurns;
    // the maximum number that can be chosen each turn
    uint maxChoice;

    // The chosen numbers
    std::vector<uint> choices;

public:
    TestGameState(uint numTurns, uint maxChoice) : numTurns(numTurns), maxChoice(maxChoice) {}

    void addChoice(uint choice) {
        choices.push_back(choice);
    }

    uint getNumTurns() const {
        return numTurns;
    }

    uint getMaxChoice() const {
        return maxChoice;
    }

    const std::vector<uint> &getChoices() const {
        return choices;
    }

protected:
    void print(std::ostream &strm) override {
        for (auto& choice : choices) {
            strm << choice << "|";
        }
    }

};

class TestGameAction : public Action<TestGameState> {
private:
    uint choice = 0;

public:
    TestGameAction() = default;

    explicit TestGameAction(uint choice) : choice(choice) {}

    void execute(TestGameState *state) override {
        state->addChoice(choice);
    }

    void setChoice(uint newChoice) {
        this->choice = newChoice;
    }

protected:
    void print(std::ostream &strm) override {
        strm << choice;
    }
};

class TestGameExpansionStrategy : public ExpansionStrategy<TestGameState, TestGameAction> {
private:
    uint currentChoice = 0;

public:
    explicit TestGameExpansionStrategy(TestGameState *mockState) : ExpansionStrategy(mockState) {}

    TestGameAction *generateNext() override {
        return new TestGameAction(currentChoice++);
    }

    bool canGenerateNext() override {
        return currentChoice <= state->getMaxChoice();
    }
};

class TestGamePlayoutStrategy : public PlayoutStrategy<TestGameState, TestGameAction> {
private:
    std::mt19937 generator;
    std::uniform_int_distribution<uint> distribution;

public:
#pragma clang diagnostic push
#pragma ide diagnostic ignored "cert-msc51-cpp"
    explicit TestGamePlayoutStrategy(TestGameState *state) : PlayoutStrategy(state),
                                                             generator(42), // We want tests to be deterministic, so pick a constant seed
                                                             distribution(0, state->getMaxChoice()) {}
#pragma clang diagnostic pop

    void generateRandom(TestGameAction *action) override {
        action->setChoice(distribution(generator));
    }
};

class TestGameScoring : public Scoring<TestGameState> {
private:
    std::vector<uint> correctNumbers;

public:
    explicit TestGameScoring(std::vector<uint> correctNumbers) : correctNumbers(std::move(correctNumbers)) {}

    /**
     * Score the chosen numbers in the sequence.
     *
     * Each correct number is worth 1/m points, where m is the length of the sequence.
     *
     * @param state the state to score
     * @return the score
     */
    float score(TestGameState *state) override {
        const auto &choices = state->getChoices();
        uint difference = 0;
        for (int i = 0; i < choices.size(); i++) {
            if (choices[i] != correctNumbers[i]) {
                difference++;
            }
        }
        return (float) (choices.size() - difference) / choices.size();
    }
};

class TestGameBackPropagation : public Backpropagation<TestGameState> {
public:
    float updateScore(TestGameState *state, float backpropScore) override {
        return backpropScore;
    }
};

class TestGameTerminationCheck : public TerminationCheck<TestGameState> {
public:
    bool isTerminal(TestGameState *state) override {
        return state->getChoices().size() == state->getNumTurns();
    }
};

typedef MCTS<TestGameState, TestGameAction, TestGameExpansionStrategy, TestGamePlayoutStrategy> TestGameMCTS;

#endif //CPP_MCTS_TESTGAME_H
