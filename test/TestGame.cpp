
#include "catch2/catch.hpp"
#include "TestGame.h"

float playGame(uint numTurns, uint maxChoice, std::default_random_engine& generator) {
    auto state = new TestGameState(numTurns, maxChoice);

    std::uniform_int_distribution<uint> distribution(0,maxChoice - 1);

    std::vector<uint> expectedSequence(state->getNumTurns());
    std::generate(expectedSequence.begin(), expectedSequence.end(), [&](){return distribution(generator);});

    for (int i=0; i < state->getNumTurns(); i++) {
        TestGameMCTS mcts(new TestGameState(*state), new TestGameBackPropagation(), new TestGameTerminationCheck,  new TestGameScoring(expectedSequence));
        auto action = mcts.calculateAction();
        action->execute(state);
    }

    TestGameScoring scoring(expectedSequence);
    return scoring.score(state);
}

TEST_CASE("MCTS with default values always wins a simple game") {
    // We want the games played to be deterministic, so seed the random number generator with a constant seed
    std::default_random_engine generator(42);

    SECTION("small game tree") {
        // results in 2^10 = 1024 game states
        REQUIRE(playGame(10, 1, generator) == 1.0F);
    }

    SECTION("medium game tree") {
        // results in 3^10 = 59049 game states
        REQUIRE(playGame(10, 2, generator) == 1.0F);
    }

    SECTION("large game tree") {
        // results in 4^10 = 1048576 game states
        REQUIRE(playGame(10, 3, generator) == 1.0F);
    }

    SECTION("huge game tree") {
        // results in 5^10 = 9765625 game states
        REQUIRE(playGame(10, 4, generator) == 1.0F);
    }

    SECTION("massive game tree") {
        // results in 6^10 = 60466176 game states
        REQUIRE(playGame(10, 5, generator) == 1.0F);
    }
}