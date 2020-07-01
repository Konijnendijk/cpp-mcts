
#include "catch2/catch.hpp"
#include "TestGame.h"

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
        mcts.setMinIterations(10000);
        auto action = mcts.calculateAction();
        action->execute(state);
        delete action;
    }

    TestGameScoring scoring(expectedSequence);
    return scoring.score(state);
}

TEST_CASE("MCTS with default values always wins a simple game") {
    // We want the games played to be deterministic, so seed the random number generator with a number of constant seeds
    int seed = GENERATE(range(1, 11));

    SECTION("small game tree") {
        // results in 2^10 = 1024 game states
        REQUIRE(playGame(10, 1, seed) == 1.0F);
    }

    SECTION("large game tree") {
        // results in 6^10 = 60466176 game states
        REQUIRE(playGame(10, 5, seed) == 1.0F);
    }
}