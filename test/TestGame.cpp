
#include "catch2/catch.hpp"
#include "TestGame.h"

/**
 * Play a game with the given number of turns and maximum number to choose.
 * 
 * The resulting game has maxChoice^numTurns possible solutions.
 * 
 * The MCTS used is deterministic. It runs for a set number of iterations and TestGamePlayoutStrategy uses a constant
 * seed for generating random moves.
 * 
 * @param numTurns the number of turns (the depth of the game tree)
 * @param maxChoice the maximum number per choice (the number of children per game tree node)
 * @param seed the seed for the generator used to generate the sequence MCTS should guess.
 * @return the score MCTS achieved. 
 */
float playGame(uint numTurns, uint maxChoice, int seed) {
    auto state = new TestGameState(numTurns, maxChoice);

    std::mt19937 generator(seed);
    std::uniform_int_distribution<uint> distribution(0, maxChoice);

    std::vector<uint> expectedSequence(state->getNumTurns());
    for (auto &entry : expectedSequence) {
        entry = distribution(generator);
    }

    for (int i = 0; i < state->getNumTurns(); i++) {
        auto copiedState = new TestGameState(*state);
        auto propagation = new TestGameBackPropagation();
        auto terminationCheck = new TestGameTerminationCheck();
        auto scoring = new TestGameScoring(expectedSequence);
        TestGameMCTS mcts(copiedState,
                          propagation,
                          terminationCheck,
                          scoring);
        // Make MCTS deterministic by setting a required number of iterations instead of a time
        mcts.setTime(0);
        mcts.setMinIterations(TEST_GAME_MCTS_ITERATIONS);
        auto action = mcts.calculateAction();
        action->execute(state);
        delete action;
    }

    TestGameScoring scoring(expectedSequence);
    return scoring.score(state);
}

TEST_CASE("MCTS wins a simple game") {
    // Play 10 games, to have more certainty that MCTS always wins
    // We want the games played to be the same across test runs, so seed the random number generator with a number of constant seeds
    int seed = GENERATE(range(1, 11));

    SECTION("small game tree") {
        // results in 2^10 = 1024 possible solutions
        REQUIRE(playGame(10, 1, seed) == 1.0F);
    }

    SECTION("large game tree") {
        // results in 6^10 = 60466176 possible solutions
        REQUIRE(playGame(10, 5, seed) == 1.0F);
    }
}