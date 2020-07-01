
#include "catch2/catch.hpp"
#include "mcts/mcts.h"
#include "Mocks.h"

typedef Node<MockState, MockAction, MockExpansionStrategy> MockNode;

MockNode* buildMockNode(unsigned int id, MockNode* parent) {
    auto state = new MockState();
    auto action = new MockAction();
    return new MockNode(id, state, parent, action);
}

TEST_CASE("nodes can have their scores updated") {
    auto node = buildMockNode(1, nullptr);

    REQUIRE(node->getNumVisits() == 0);
    REQUIRE(isnanf(node->getAvgScore()));

    SECTION("updating scores") {
        node->update(0.5F);

        REQUIRE(node->getNumVisits() == 1);
        REQUIRE(node->getAvgScore() == 0.5F);

        node->update(1.0F);

        REQUIRE(node->getNumVisits() == 2);
        REQUIRE(node->getAvgScore() == Approx(0.75F));
    }

    delete node;
}

TEST_CASE("nodes can build a tree") {
    auto root = buildMockNode(1, nullptr);
    auto childA = buildMockNode(2, root);
    auto childB = buildMockNode(3, root);

    REQUIRE(root->getChildren().empty());

    SECTION("Add children") {
        root->addChild(childA);
        root->addChild(childB);

        REQUIRE(root->getChildren() == std::vector<MockNode*>{childA, childB});
    }
}
