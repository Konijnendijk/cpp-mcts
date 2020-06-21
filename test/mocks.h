
#ifndef CPP_MCTS_MOCKS_H
#define CPP_MCTS_MOCKS_H

class MockState: public State {

};

class MockAction: public Action<MockState> {
    void execute(MockState *state) override {}
};

class MockExpansionStrategy: public ExpansionStrategy<MockState, MockAction> {
public:
    explicit MockExpansionStrategy(MockState *state) : ExpansionStrategy(state) {}

private:

    MockAction *generateNext() override {
        return nullptr;
    }

    bool canGenerateNext() override {
        return false;
    }
};

#endif //CPP_MCTS_MOCKS_H
