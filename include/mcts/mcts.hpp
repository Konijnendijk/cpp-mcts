#include <chrono>
#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <vector>

#ifndef CPP_MCTS_MCTS_HPP
#define CPP_MCTS_MCTS_HPP

/**
 * @brief Children of this class should represent game states
 *
 * A game state is the representation of a single point in the game. For
 * instance in chess, it should at least store all pieces and their locations.
 */
class State {
    friend std::ostream& operator<<(std::ostream& strm, State& s)
    {
        s.print(strm);
        return strm;
    };

protected:
    /**
     * @brief Print a human-readable representation of this state
     *
     * Used for debugging MCTS. Implementations should print a human-readable
     * representation of themselves to the provided stream strm. This
     * representation will also be used when outputting a .dot file using the
     * writeDotFile function from graphviz.h.
     *
     * @param strm The stream to print to
     */
    virtual void print(std::ostream& strm) { strm << this; };

public:
    virtual ~State() = default;
};

/**
 * @brief Implementations of this class should represent an action a player can
 * execute on a State.
 *
 * An action is something that acts on a State and results in another. For
 * example in chess an action could be to move the queen to g5.
 *
 * <b>Action must implement a copy constructor.</b>
 *
 * @tparam T The State type this Action can be executed on
 */
template <class T>
class Action {
    friend std::ostream& operator<<(std::ostream& strm, Action<T>& a)
    {
        a.print(strm);
        return strm;
    };

protected:
    virtual void print(std::ostream& strm) { strm << this; };

public:
    /**
     * @brief Apply this Action on the given State
     *
     * Should transform the given state to a new one according to this action. For
     * example in chess, calling execute could move the king one square.
     *
     * @note Cloning the state is not required
     * @param state The state to execute on
     */
    virtual void execute(T& state) = 0;

    virtual ~Action() = default;
};

/**
 * @brief Base class for strategies
 *
 * A strategy is a behaviour that can generate an Action depending on a State.
 */
template <class T>
class Strategy {
protected:
    /** The state a PlayoutStrategy or ExpansionStrategy will act on  */
    T* state;

public:
    explicit Strategy(T* state)
        : state(state)
    {
    }

    virtual ~Strategy() = default;
};

/**
 * @brief A strategy that lazily generates child states given the parent state
 *
 * This strategy generates actions that are used in the expansion stage of MCTS.
 *
 * @note Implementing classes must have a constructor taking only one parameter
 * of type State
 *
 * @tparam T The type of State this ExpansionStrategy can generate Actions for
 * @tparam A The type of Actions that will be generated
 */
template <class T, class A>
class ExpansionStrategy : public Strategy<T> {

public:
    explicit ExpansionStrategy(T* state)
        : Strategy<T>(state)
    {
    }

    /**
     * @brief Generate the next action in the sequence of possible ones
     *
     * Generate a action that can be performed on Strategy#state and which has not
     * been returned before by this instance of ExpansionStrategy.
     *
     * @return An Action that has not been returned before, or nullptr if no such
     * Action exists
     */
    virtual A generateNext() = 0;

    /**
     * @return True if generateNext() can generate a new Action
     */
    virtual bool canGenerateNext() const = 0;
};

/**
 * @brief Generate random actions
 *
 * This strategy generates random actions that are used in the playout stage of
 * MCTS.
 *
 * @note Implementing classes must have a constructor taking only one parameter
 * of type State
 *
 * @tparam T The type of State this PlayoutStrategy can generate Actions for
 * @tparam A The type of Actions that will be generated
 */
template <class T, class A>
class PlayoutStrategy : public Strategy<T> {

public:
    explicit PlayoutStrategy(T* state)
        : Strategy<T>(state)
    {
    }

    /**
     * @brief Generate a random action
     *
     * Generate a random Action that can be performed on Strategy#state.
     *
     * @param action the action to store the result in
     */
    virtual void generateRandom(A& action) = 0;
};

/**
 * @brief Adjusts a score being backpropagated
 *
 * When backpropagating a score through the tree it can be adjusted by this
 * class. Backpropagation::updateScore() is called before Node#update() and the
 * result of Backpropagation::updateScore() is passed to Node::update() instead
 * of the score resulting from Scoring.
 *
 * This is useful for e.g. multiplayer games. For example in chess, the score
 * for the current player should not be adjusted while the score for the enemy
 * player should be inverted (a win for the current player is a loss for the
 * enemy player).
 *
 * @tparam T The State type this Backpropagation can calculate updated scores
 * for
 */
template <class T>
class Backpropagation {

public:
    /**
     * @param state The state the score is currently being updated for
     * @param backpropScore The score being backpropagated resulting from
     * Scoring::score()
     * @return An updated score for the current state
     */
    virtual float updateScore(const T& state, float backpropScore) = 0;

    virtual ~Backpropagation() = default;
};

/**
 * @brief check if a state is terminal
 *
 * Checks if a state is terminal, i.e. the end of the game.
 *
 * @tparam T The State type this TeminationCheck can check
 */
template <class T>
class TerminationCheck {

public:
    /**
     * @return True if the given state can not haven any children, i.e. the end of
     * the game is reached
     */
    virtual bool isTerminal(const T& state) = 0;

    virtual ~TerminationCheck() = default;
};

/**
 * @brief Calculates the score of a terminal state
 *
 * Calculate the score of a terminal (i.e. end-of-game) state. A score is
 * usually a number between 0 and 1 where 1 is the best possible score. A score
 * is calculated at the end of the playout stage and is then backpropagated.
 * During backpropagation scores can be updated using Backpropagation.
 *
 * @tparam T The State type this Scoring can calculate scores for
 */
template <class T>
class Scoring {

public:
    /**
     * @brief Calculate a score for a terminal state
     *
     * A score should be high when the state represents a good end result for the
     * current player and low when the end result is poor.
     *
     * @return A score for the given state
     */
    virtual float score(const T& state) = 0;

    virtual ~Scoring() = default;
};

/**
 * @brief Class used in the internal data structure of MCTS
 *
 * A Node contains all information needed to generate children. It keeps track
 * of its score and the number of times it has been visited. Furthermore it is
 * used to generate new nodes according to the ExpansionStrategy E.
 *
 * @tparam T The State type that is stored in a node
 * @tparam A The type of Action taken to get to this node
 * @tparam E The ExpansionStrategy to use when generating new nodes
 */
template <class T, class A, class E>
class Node {
    unsigned int id;
    T data;
    std::shared_ptr<Node<T, A, E>> parent;
    std::vector<std::shared_ptr<Node<T, A, E>>> children;
    /** Action done to get from the parent to this node */
    A action;
    E expansion;
    int numVisits = 0;
    float scoreSum = 0.0F;

public:
    /**
     * @brief Create a new node in the search tree
     *
     * This constructor initializes the nodes and creates a new instance of the
     * ExpansionStrategy passed as template parameter E.
     *
     * @param id An identifier unique to the tree this node is in
     * @param data The state stored in this node
     * @param parent The parent node
     * @param action The action taken to get to this node from the parent node
     */
    Node(unsigned int id, T data, std::shared_ptr<Node<T, A, E>> parent, A action)
        : id(id)
        , data(std::move(data))
        , parent(parent)
        , action(std::move(action))
        , expansion(&this->data)
    {
    }

    /**
     * @return The unique ID of this node
     */
    unsigned int getID() const { return id; }

    /**
     * @return The State associated with this Node
     */
    const T& getData() const { return data; }

    /**
     * @return This Node's parent or nullptr if no parent exists (this Node is the
     * root)
     */
    std::shared_ptr<Node<T, A, E>> getParent() const { return parent; }

    /**
     * @return All children of this Node
     */
    const std::vector<std::shared_ptr<Node<T, A, E>>>& getChildren() const { return children; }

    /**
     * @return The Action to execute on the parent's State to get from the
     * parent's State to this Node's State.
     */
    const A& getAction() const { return action; }

    /**
     * @return A new action if there are any remaining, nullptr if not
     */
    A generateNextAction() { return expansion.generateNext(); }

    /**
     * @brief Add a child to this Node's children
     * @param child The child to add
     */
    void addChild(const std::shared_ptr<Node<T, A, E>>& child) { children.push_back(child); }

    /**
     * @brief Checks this Node's ActionGenerator if there are more Actions to be
     * generated.
     * @return True if it is still possible to add children
     */
    bool shouldExpand() const
    {
        return children.empty() || expansion.canGenerateNext();
    }

    /**
     * @brief Update this Node's score and increment the number of visits.
     * @param score
     */
    void update(float score)
    {
        this->scoreSum += score;
        numVisits++;
    }

    /**
     * @return The total score divided by the number of visits.
     */
    float getAvgScore() const { return scoreSum / numVisits; }

    /**
     * @return The number of times updateScore(score) was called
     */
    int getNumVisits() const { return numVisits; }
};

/**
 * @brief AI search technique for finding the best Action give a certain State
 *
 * The MCTS algorithm has four stages: selection, expansion, playout and
 * backpropagation. This class represents the general framework for executing
 * these stages and uses a number of user-implemented classes which implement
 * the game rules.
 *
 * In the selection stage, MCTS uses the UCT formula to select the best node (or
 * randomly if a node has not been visited often enough, see
 * MCTS::setMinVisits()) until it finds a node that still has nodes left to be
 * expanded. The UCT formula has one parameter, see MCTS::setC(). When PROG_HIST
 * is defined, the progressive history heuristic is used to influence the
 * selection based on the success of an action during the playout stage.
 * MCTS::setW() is used to set the W parameter for progressive history.
 *
 * In the expansion stage an action is requested from the ExpansionStrategy and
 * a node is expanded using that action. When a node is not visited at least T
 * times, expansion is skipped (see MCTS::setMinT()).
 *
 * In the playout stage, the PlayoutStrategy is used to generate moves until the
 * end of the game is reached. When a terminal state (the end of the game) is
 * encoutered, the score is calculated using Scoring.
 *
 * In the backpropagation stage, Node::update() is called for each node from the
 * node expanded in the expansion stage to the root node. The score passed to
 * Node::update() is the one from the call to Scoring::score() passed to
 * Backpropagation::updateScore() for each call to Node::update().
 *
 * The time that MCTS is allowed to search van be set by MCTS::setTime().
 *
 * @tparam T The State type this MCTS operates on
 * @tparam A The Action type this MCTS operates on
 * @tparam E The ExpansionStrategy this MCTS uses
 * @tparam P The PlayoutStrategy this MCTS uses
 */
template <class T, class A, class E, class P>
class MCTS {
    /** Default thinking time in milliseconds */
    const int DEFAULT_TIME = 500;

    /** MCTS can go over time if it has less than this amount of iterations */
    const int DEFAULT_MIN_ITERATIONS = 0;

    /** Default C for the UCT formula */
    static constexpr float DEFAULT_C = 0.5;

    /** Minimum number of visits until a Node will be expanded */
    const int DEFAULT_MIN_T = 5;

    /** Default number of visits until a node can be selected using UCT instead of
     * randomly */
    const int DEFAULT_MIN_VISITS = 5;

    Backpropagation<T>* backprop;
    TerminationCheck<T>* termination;
    Scoring<T>* scoring;

    std::shared_ptr<Node<T, A, E>> root;

    /** The time MCTS is allowed to search */
    std::chrono::milliseconds allowedComputationTime = std::chrono::milliseconds(DEFAULT_TIME);

    /** MCTS can go over time if it has less than this amount of iterations */
    int minIterations = DEFAULT_MIN_ITERATIONS;

    /** Tunable bias parameter for node selection */
    float C = DEFAULT_C;

    /** Minimum number of visits until a Node will be expanded */
    int minT = DEFAULT_MIN_T;

    /** Minimum number of visits until a Node will be selected using the UCT
     * formula, below this number random selection is used */
    int minVisits = DEFAULT_MIN_VISITS;

    /** Variable to assign IDs to a node */
    unsigned int currentNodeID = 0;

    /** The number of search iterations so far */
    unsigned int iterations = 0;

    /** Random generator used in node selection */
    std::mt19937 generator;

public:
    /**
     * @note backprop, termination and scoring will be deleted by this MCTS
     * instance
     */
    MCTS(const T& rootData, Backpropagation<T>* backprop, TerminationCheck<T>* termination, Scoring<T>* scoring)
        : backprop(backprop)
        , termination(termination)
        , scoring(scoring)
        , root(std::make_shared<Node<T, A, E>>(0, rootData, nullptr, A()))
    {
    }

    MCTS(const MCTS& other) = default;
    MCTS(MCTS&& other)
    noexcept = default;

    MCTS<T, A, E, P>& operator=(const MCTS<T, A, E, P>& other) = default;
    MCTS<T, A, E, P>& operator=(MCTS<T, A, E, P>&& other) noexcept = default;

    /**
     * @brief Runs the MCTS algorithm and searches for the best Action
     *
     * @return The Action found by MCTS
     */
    A calculateAction()
    {
        search();

        // Select the Action with the best score
        std::shared_ptr<Node<T, A, E>> best;
        float bestScore = -std::numeric_limits<float>::max();
        auto& children = root->getChildren();

        for (unsigned int i = 0; i < children.size(); i++) {
            float score = children[i]->getAvgScore();
            if (score > bestScore) {
                bestScore = score;
                best = children[i];
            }
        }

        // If no expansion took place, simply execute a random action
        if (!best) {
            A action;
            T state(root->getData());
            auto playout = P(&state);
            playout.generateRandom(action);
            return action;
        }

        return best->getAction();
    }

    /**
     * Set the allowed computation time in milliseconds
     * @param time In milliseconds
     */
    void setTime(int time) { this->allowedComputationTime = std::chrono::milliseconds(time); }

    /**
     * @brief Set the C parameter of the UCT formula
     * @param newC The C parameter
     */
    void setC(float newC) { this->C = newC; }

    /**
     * @brief Set the minimal number of visits until a node is expanded
     * @param newMinT the minimal number of visits
     */
    void setMinT(float newMinT) { this->minT = newMinT; }

    /**
     * Set the minimum number of visits until UCT is used instead of random
     * selection during the selection stage.
     * @param newMinVisits The minimal number of visits
     */
    void setMinVisits(int newMinVisits) { this->minVisits = newMinVisits; }

    /**
     * Set the minimum number of iterations required before calculateAction()
     * returns.
     *
     * MCTS will go over time, set using setTime(int), if this number of
     * iterations is not reached.
     *
     * @param minVisits The minimum number of iterations
     */
    void setMinIterations(int i) { this->minIterations = i; }

    /**
     * Get the root of the MCTS tree. Useful for printing.
     * @see writeDotFile()
     * @return The root of the MCTS tree
     */
    Node<T, A, E>& getRoot() { return root; }

    ~MCTS()
    {
        delete backprop;
        delete termination;
        delete scoring;
    }

private:
    void search()
    {
        std::chrono::system_clock::time_point old = std::chrono::system_clock::now();

        while (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - old) < allowedComputationTime || iterations < minIterations) {
            iterations++;

            /**
             * Selection
             */
            std::shared_ptr<Node<T, A, E>> selected = root;
            while (!selected->shouldExpand())
                selected = select(*selected);

            if (termination->isTerminal(selected->getData())) {
                backProp(*selected, scoring->score(selected->getData()));
                continue;
            }

            /**
             * Expansion
             */
            std::shared_ptr<Node<T, A, E>> expanded;
            int numVisits = selected->getNumVisits();
            if (numVisits >= minT) {
                expanded = expandNext(selected);
            } else {
                expanded = selected;
            }

            /**
             * Simulation
             */
            simulate(*expanded);
        }
    }

    /** Selects the best child node at the given node */
    std::shared_ptr<Node<T, A, E>> select(const Node<T, A, E>& node)
    {
        std::shared_ptr<Node<T, A, E>> best = nullptr;
        float bestScore = -std::numeric_limits<float>::max();

        auto& children = node.getChildren();

        // Select randomly if the Node has not been visited often enough
        if (node.getNumVisits() < minVisits) {
            std::uniform_int_distribution<uint> distribution(0, children.size() - 1);
            return children[distribution(generator)];
        }

        // Use the UCT formula for selection
        for (auto& n : children) {
            float score = n->getAvgScore() + C * (float)sqrt(log(node.getNumVisits()) / n->getNumVisits());

            if (score > bestScore) {
                bestScore = score;
                best = n;
            }
        }

        return best;
    }
    /** Get the next Action for the given Node, execute and add the new Node to
     * the tree. */
    std::shared_ptr<Node<T, A, E>> expandNext(const std::shared_ptr<Node<T, A, E>>& node)
    {
        T expandedData(node->getData());
        auto action = node->generateNextAction();
        action.execute(expandedData);
        auto newNode = std::make_shared<Node<T, A, E>>(++currentNodeID, expandedData, node, action);
        node->addChild(newNode);
        return newNode;
    }

    /** Simulate until the stopping condition is reached. */
    void simulate(Node<T, A, E>& node)
    {
        T state(node.getData());

        A action;
        // Check if the end of the game is reached and generate the next state if
        // not
        while (!termination->isTerminal(state)) {
            P playout(&state);
            playout.generateRandom(action);
            action.execute(state);
        }

        // Score the leaf node (end of the game)
        float s = scoring->score(state);

        backProp(node, s);
    }

    /** Backpropagate a score through the tree */
    void backProp(Node<T, A, E>& node, float score)
    {
        node.update(backprop->updateScore(node.getData(), score));

        std::shared_ptr<Node<T, A, E>> current = node.getParent();
        while (current) {
            current->update(backprop->updateScore(current->getData(), score));
            current = current->getParent();
        }
    }
};

#endif // CPP_MCTS_MCTS_HPP
