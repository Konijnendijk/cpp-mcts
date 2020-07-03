
#ifndef CPP_MCTS_MOCKS_HPP
#define CPP_MCTS_MOCKS_HPP

#include "mcts/mcts.hpp"

class MockState : public State {
};

class MockAction : public Action<MockState> {
    void execute(MockState* state) override
    {
        // Mock action, stub implementation
    }
};

class MockExpansionStrategy : public ExpansionStrategy<MockState, MockAction> {
public:
    explicit MockExpansionStrategy(MockState* mockState)
        : ExpansionStrategy(mockState)
    {
    }

private:
    MockAction generateNext() override { return MockAction(); }

    bool canGenerateNext() override { return false; }
};

#endif // CPP_MCTS_MOCKS_HPP
