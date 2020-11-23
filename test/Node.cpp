
#include "Mocks.hpp"
#include "catch2/catch.hpp"
#include "mcts/mcts.hpp"

using MockNode = Node<MockState, MockAction, MockExpansionStrategy>;

std::shared_ptr<MockNode> buildMockNode(unsigned int id, std::shared_ptr<MockNode> parent)
{
    return std::make_shared<MockNode>(id, MockState(), parent, MockAction());
}

TEST_CASE("nodes can have their scores updated")
{
    auto node = buildMockNode(1, nullptr);

    REQUIRE(node->getNumVisits() == 0);
    REQUIRE(isnan(node->getAvgScore()));

    SECTION("updating scores")
    {
        node->update(0.5F);

        REQUIRE(node->getNumVisits() == 1);
        REQUIRE(node->getAvgScore() == 0.5F);

        node->update(1.0F);

        REQUIRE(node->getNumVisits() == 2);
        REQUIRE(node->getAvgScore() == Approx(0.75F));
    }
}

TEST_CASE("nodes can build a tree")
{
    auto root = buildMockNode(1, nullptr);
    auto childA = buildMockNode(2, root);
    auto childB = buildMockNode(3, root);

    REQUIRE(root->getChildren().empty());

    SECTION("Add children")
    {
        root->addChild(childA);
        root->addChild(childB);

        REQUIRE(root->getChildren() == std::vector<std::shared_ptr<MockNode>> { childA, childB });
    }
}
