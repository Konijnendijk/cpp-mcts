#include <vector>
#include <unordered_map>
#include <limits>
#include <cmath>
#include <iostream>
#include <chrono>
#include <cstdlib>

#ifndef MCTS_H
#define MCTS_H

using namespace std::chrono;

/**
 * @brief Children of this class should represent game states
 *
 * A game state is the representation of a single point in the game. For instance in chess, it should at least store all
 * pieces and their locations.
 */
class State{
	friend std::ostream& operator<<(std::ostream& strm, State& s){
		s.print(strm);
	    return strm;
	};

protected:
    /**
     * @brief Print a human-readable representation of this state
     *
     * Used for debugging MCTS. Implementations should print a human-readable representation of themselves
     * to the provided stream strm. This representation will also be used when outputting a .dot file using
     * the writeDotFile function from graphviz.h.
     *
     * @param strm The stream to print to
     */
	virtual void print(std::ostream &strm) {strm << this;};

public:
	virtual ~State(){};
};

/**
 * @brief Implementations of this class should represent an action a player can execute on a State.
 *
 * An action is something that acts on a State and results in another. For example in chess
 *
 * <b>Action must implement a copy constructor.</b>
 *
 * @tparam The State type this Action can be executed on
 */
template<class T>
class Action{
	friend std::ostream& operator<< (std::ostream& strm, Action<T>& a){
		a.print(strm);
	    return strm;
	};

protected:
	virtual void print(std::ostream &strm) {strm << this;};

public:
	/**
	 * @brief Apply this Action on the given State
	 *
	 * Should transform the given state to a new one according to this action. For example in chess, calling execute
	 * could move the king one square.
	 *
	 * @note Cloning the state is not required
	 * @param state The state to execute on
	 */
	virtual void execute(T* state)=0;

	/**
	 * @brief Calculate a hash of this action
	 *
	 * @noteImplementing this is only required when using progressive history. See MCTS for more details.
	 * @return The unique (over all possible actions in the game) hash of this action
	 */
	virtual size_t hash() {return 0;};

	/**
	 * @brief Returns true when this Action equals the given action
	 *
     * @note Implementing this is only required when using progressive history. See MCTS for more details.
	 * @return True if this and other are equal, false if not
	 */
	virtual bool equals(Action<T>* other) { return this==other; };

	virtual ~Action(){};
};

namespace std {
    /**
     * Hash specialization for Action pointers used in the Progressive History technique in MCTS.
     */
    template<class T>
    class hash<Action<T>*>{
    public:
        size_t operator() (Action<T>* const & a) const {
            return a->hash();
        }
    };

    /**
     * equal_to specialization for Action pointers used in the Progressive History technique in MCTS.
     */
    template<class T>
    class equal_to<Action<T>*>{
    public :
        bool operator() (Action<T>* const & x, Action<T>* const & y) const {return x->equals(y);}
    };
}

/**
 * @brief Base class for strategies
 *
 * A strategy is a behaviour that can generate an Action depending on a State.
 */
template<class T>
class Strategy{
protected:
    /** The state a PlayoutStrategy or ExpansionStrategy will act on  */
	T* state;

public:
	Strategy(T* state) : state(state) {}

	virtual ~Strategy(){};
};

/**
 * @brief A strategy that lazily generates child states given the parent state
 *
 * This strategy generates actions that are used in the expansion stage of MCTS.
 *
 * @note Implementing classes must have a constructor taking only one parameter of type State
 *
 * @tparam T The type of State this ExpansionStrategy can generate Actions for
 * @tparam A The type of Actions that will be generated
 */
template<class T, class A>
class ExpansionStrategy : public Strategy<T> {

public:
	ExpansionStrategy(T* state) : Strategy<T>(state) {}

	/**
	 * @brief Generate the next action in the sequence of possible ones
	 *
	 * Generate a action that can be performed on Strategy#state and which has not been returned before by this
	 * instance of ExpansionStrategy.
	 *
	 * @return An Action that has not been returned before, or nullptr if no such Action exists
	 */
	virtual A* generateNext()=0;

	/**
	 * @return True if generateNext() can generate a new Action
	 */
	virtual bool canGenerateNext()=0;

	virtual ~ExpansionStrategy() override {}
};

/**
 * @brief Generate random actions
 *
 * This strategy generates random actions that are used in the playout stage of MCTS.
 *
 * @note Implementing classes must have a constructor taking only one parameter of type State
 * 
 * @tparam T The type of State this PlayoutStrategy can generate Actions for
 * @tparam A The type of Actions that will be generated
 */
template<class T, class A>
class PlayoutStrategy : public Strategy<T>{

public:
	PlayoutStrategy(T* state) : Strategy<T>(state) {}

	/**
	 * @brief Generate a random action
     * Generate a random Action that can be performed on Strategy#state.
     *
	 * @return A random Action that can be executed on Strategy#state
	 */
	virtual void generateRandom(A* action)=0;

	virtual ~PlayoutStrategy() override {}
};

/**
 * @brief Adjusts a score being backpropagated
 *
 * When backpropagating a score through the tree it can be adjusted by this class. Backpropagation::updateScore() is
 * called before Node#update() and the result of Backpropagation::updateScore() is passed to Node::update() instead of the
 * score resulting from Scoring.
 *
 * This is useful for e.g. multiplayer games. For example in chess, the score for the current player should not be
 * adjusted while the score for the enemy player should be inverted (a win for the current player is a loss for the
 * enemy player).
 *
 * @tparam T The State type this Backpropagation can calculate updated scores for
 */
template<class T>
class Backpropagation  {

public:
	Backpropagation() {}

	/**
	 * @param state The state the score is currently being updated for
	 * @param backpropScore The score being backpropagated resulting from Scoring::score()
	 * @return An updated score for the current state
	 */
	virtual float updateScore(T* state, float backpropScore)=0;

	virtual ~Backpropagation() {}
};

/**
 * @brief check if a state is terminal
 *
 * Checks if a state is terminal, i.e. the end of the game.
 *
 * @tparam T The State type this TeminationCheck can check
 */
template<class T>
class TerminationCheck{

public:
	TerminationCheck() {}

    /**
     * @return True if the given state can not haven any children, i.e. the end of the game is reached
     */
	virtual bool isTerminal(T* state)=0;

	virtual ~TerminationCheck(){}
};

/**
 * @brief Calculates the score of a terminal state
 *
 * Calculate the score of a terminal (i.e. end-of-game) state. A score is usually a number between 0 and 1 where 1 is
 * the best possible score. A score is calculated at the end of the playout stage and is then backpropagated. During
 * backpropagation scores can be updated using Backpropagation.
 *
 * @tparam T The State type this Scoring can calculate scores for
 */
template<class T>
class Scoring {

public:
	Scoring() {}

    /**
     * @brief Calculate a score for a terminal state
     *
     * A score should be high when the state represents a good end result for the current player and low when
     * the end result is poor.
     *
     * @return A score for the given state
     */
	virtual float score(T* state)=0;

	virtual ~Scoring(){}
};


/**
 * @brief Class used in the internal data structure of MCTS
 *
 * A Node contains all information needed to generate children. It keeps track of its score and the number of times
 * it has been visited. Furthermore it is used to generate new nodes according to the ExpansionStrategy E.
 *
 * @tparam T The State type that is stored in a node
 * @tparam A The type of Action taken to get to this node
 * @tparam E The ExpansionStrategy to use when generating new nodes
 */
template<class T, class A, class E>
class Node{
	unsigned int id;
	T* data;
	Node<T,A,E>* parent;
	std::vector<Node<T,A,E>*> children;
	/** Action done to get from the parent to this node */
	A* action;
	ExpansionStrategy<T,A>* expansion;
	int numVisits;
	float score;

public:
    /**
     * @brief Create a new node in the search tree
     *
     * This constructor initializes the nodes and creates a new instance of the ExpansionStrategy passed as template
     * parameter E.
     *
     * @param id An identifier unique to the tree this node is in
     * @param data The state stored in this node
     * @param parent The parent node
     * @param action The action taken to get to this node from the parent node
     */
	Node(unsigned int id, T* data, Node<T,A,E>* parent, A* action) : id(id), data(data), parent(parent), action(action), expansion(new E(data)), numVisits(0), score(0) {
	};

	/**
	 * @return The unique ID of this node
	 */
	unsigned int getID(){
		return id;
	}

	/**
	 * @return The State associated with this Node
	 */
	T* getData(){
		return data;
	}

	/**
	 * @return This Node's parent or nullptr if no parent exists (this Node is the root)
	 */
	Node<T,A,E>* getParent(){
		return parent;
	}

	/**
	 * @return All children of this Node
	 */
	std::vector<Node<T,A,E>*>& getChildren(){
		return children;
	}

	/**
	 * @return The Action to execute on the parent's State to get from the parent's State to this Node's State.
	 */
	A* getAction(){
		return action;
	}

	/**
	 * @return A new action if there are any remaining, nullptr if not
	 */
	A* generateNextAction(){
		return expansion->generateNext();
	}

	/**
	 * @brief Add a child to this Node's children
	 * @param child The child to add
	 */
	void addChild(Node<T,A,E>* child){
		children.push_back(child);
	}

	/**
	 * @brief Checks this Node's ActionGenerator if there are more Actions to be generated.
	 * @return True if it is still possible to add children
	 */
	bool shouldExpand(){
		bool result=children.empty() || expansion->canGenerateNext();
		return result;
	}

	/**
	 * @brief Update this Node's score and increment the number of visits.
	 * @param score
	 */
	void update(float score){
		this->score += score;
		numVisits++;
	}

	/**
	 * @return The total score divided by the number of visits.
	 */
	float getAvgScore(){
		return score/numVisits;
	}

	/**
	 * @return The number of times updateScore(score) was called
	 */
	int getNumVisits(){
		return numVisits;
	}

	~Node(){
		delete data;
		delete action;
		delete expansion;
		for (Node<T,A,E>* child : children)
			delete child;
	}
};

/**
 * @brief AI search technique for finding the best Action give a certain State
 *
 * The MCTS algorithm has four stages: selection, expansion, playout and backpropagation. This class represents the
 * general framework for executing these stages and uses a number of user-implemented classes which implement the game
 * rules.
 *
 * In the selection stage, MCTS uses the UCT formula to select the best node (or randomly if a node has not been
 * visited often enough, see MCTS::setMinVisits()) until it finds a node that still has nodes left to be expanded. The
 * UCT formula has one parameter, see MCTS::setC(). When PROG_HIST is defined, the progressive history heuristic is used
 * to influence the selection based on the success of an action during the playout stage. MCTS::setW() is used to set
 * the W parameter for progressive history.
 *
 * In the expansion stage an action is requested from the ExpansionStrategy and a node is expanded using that action.
 * When a node is not visited at least T times, expansion is skipped (see MCTS::setMinT()).
 *
 * In the playout stage, the PlayoutStrategy is used to generate moves until the end of the game is reached. When a
 * terminal state (the end of the game) is encoutered, the score is calculated using Scoring.
 *
 * In the backpropagation stage, Node::update() is called for each node from the node expanded in the expansion stage
 * to the root node. The score passed to Node::update() is the one from the call to Scoring::score() passed to
 * Backpropagation::updateScore() for each call to Node::update().
 *
 * The time that MCTS is allowed to search van be set by MCTS::setTime().
 *
 * @tparam T The State type this MCTS operates on
 * @tparam A The Action type this MCTS operates on
 * @tparam E The ExpansionStrategy this MCTS uses
 * @tparam P The PlayoutStrategy this MCTS uses
 */
template<class T, class A, class E, class P>
class MCTS {
	/** Default thinking time in milliseconds */
	const int DEFAULT_TIME=500;

	/** MCTS can go over time if it has less than this amount of iterations */
	const int DEFAULT_MIN_ITERATIONS=0;

	/** Default C for the UCT formula */
	static constexpr float DEFAULT_C=0.5;

	/** Default W for the progressive history formula */
	static constexpr float DEFAULT_W=0.0;

	/** Minimum number of visits until a Node will be expanded */
	const int DEFAULT_MIN_T=5;

	/** Default number of visits until a node can be selected using UCT instead of randomly */
	const int DEFAULT_MIN_VISITS=5;

	Backpropagation<T>* backprop;
	TerminationCheck<T>* termination;
	Scoring<T>* scoring;

	Node<T,A,E>* root;

	/** Map holding the information for use in the Progressive History technique.
	 * The value holds the number of times an Action was done and the score that action led to.
	 */
	std::unordered_map<Action<T>*, std::pair<int, float>> history;

	/** The time MCTS is allowed to search */
	milliseconds time;

	/** MCTS can go over time if it has less than this amount of iterations */
	int minIterations;

	/** Tunable bias parameter for node selection */
	float C;

	/** Tunable parameter determining the influence of history */
	float W;

	/** Minimum number of visits until a Node will be expanded */
	int minT;

	/** Minimum number of visits until a Node will be selected using the UCT formula, below this number random selection is used */
	int minVisits;

	/** Variable to assign IDs to a node */
	unsigned int currentNodeID;

	/*
	 * Debugging variables
	 */
	microseconds selectTime, expandTime,simulateTime;
	long iterations;

public:
	/**
	 * @note backprop, termination and scoring will be deleted by this MCTS instance
	 */
	MCTS(T* rootData, Backpropagation<T>* backprop, TerminationCheck<T>* termination, Scoring<T>* scoring) :
	    backprop(backprop), termination(termination), scoring(scoring), root(new Node<T,A,E>(0, rootData, 0, new A())),
	        history(), time(milliseconds(DEFAULT_TIME)), minIterations(DEFAULT_MIN_ITERATIONS), C(DEFAULT_C),
	            W(DEFAULT_W), minT(DEFAULT_MIN_T), minVisits(DEFAULT_MIN_VISITS), currentNodeID(0),
	                selectTime(microseconds::zero()), expandTime(microseconds::zero()),
	                    simulateTime(microseconds::zero()), iterations(0) {}

	/**
	 * @brief Runs the MCTS algorithm and searches for the best Action
	 *
	 * @return The Action found by MCTS
	 */
	A* calculateAction(){

		system_clock::time_point old=system_clock::now();

		search();

        #ifdef _DEBUG
		std::cerr << iterations << " iterations in " << duration_cast<milliseconds>(system_clock::now()-old).count() << "ms" << std::endl;
		std::cerr << "Average select:" << (float)(selectTime.count()/1000)/iterations/1000 << "ms Average expand:" << (float)(expandTime.count())/iterations/1000 << "ms Average simulate:" << (float)(simulateTime.count())/iterations/1000 << "ms" << std::endl;
		#endif

		// Select the Action with the best score
		Node<T,A,E>* best=nullptr;
		float bestScore=-std::numeric_limits<float>::max();
		std::vector<Node<T,A,E>*>& children=root->getChildren();

		for (unsigned int i=0; i<children.size();i++){
			float score=children[i]->getAvgScore();
			if (score>bestScore){
				bestScore=score;
				best=children[i];
			}
		}

        for (auto kv : history)
            std::cout << kv.first->hash() << " " << kv.second.first << std::endl;

		return new A(*best->getAction());
	}

	/**
	 * Set the allowed computation time in milliseconds
	 * @param time In milliseconds
	 */
	void setTime(int time){
		this->time=milliseconds(time);
	}

	/**
	 * @brief Set the C parameter of the UCT formula
	 * @param C The C parameter
	 */
	void setC(float C){
		this->C=C;
	}

    /**
     * @brief Set the W parameter of the progressive history heuristic.
     * @param W the W parameter
     */
	void setW(float W){
		this->W=W;
	}

    /**
     * @brief Set the minimal number of visits until a node is expanded
     * @param minT the minimal number of visits
     */
	void setMinT(float minT){
		this->minT = minT;
	}

    /**
     * Set the minimum number of visits until UCT is used instead of random selection during the selection stage.
     * @param minVisits The minimal number of visits
     */
	void setMinVisits(int minVisits) {
	    this->minVisits = minVisits;
	}

    /**
     * Set the minimum number of iterations required before calculateAction() returns.
     *
     * MCTS will go over time, set using setTime(int), if this number of iterations is not reached.
     *
     * @param minVisits The minimum number of iterations
     */
    void setMinIterations(int i) {
        this->minIterations = i;
    }

    /**
     * Get the root of the MCTS tree. Useful for printing.
     * @see writeDotFile()
     * @return The root of the MCTS tree
     */
	Node<T,A,E>* getRoot(){
		return root;
	}

	~MCTS(){
		for (auto kv : history)
			delete kv.first;

		delete root;
		delete backprop;
		delete termination;
		delete scoring;
	}
private:
	void search(){
		system_clock::time_point old=system_clock::now();


		#ifdef _DEBUG
		system_clock::time_point temp;
		#endif

		while (duration_cast<milliseconds>(system_clock::now()-old)<time || iterations < minIterations){

			#ifdef _DEBUG
			temp=system_clock::now();
			#endif


			iterations++;

			/**
			 * Selection
			 */
			Node<T,A,E>* selected=root;
			while(!selected->shouldExpand())
				selected=select(selected);

			if (termination->isTerminal(selected->getData())){
				backProp(selected, scoring->score(selected->getData()));
				continue;
			}

			#ifdef _DEBUG
			selectTime+=duration_cast<microseconds>(system_clock::now()-temp);
			temp=system_clock::now();
			#endif

			/**
			 * Expansion
			 */
			Node<T,A,E>* expanded;
			int numVisits=selected->getNumVisits();
			if (numVisits>=minT){
				expanded=expandNext(selected);
			}
			else{
				expanded=selected;
			}

			#ifdef _DEBUG
			expandTime+=duration_cast<microseconds>(system_clock::now()-temp);
			temp=system_clock::now();
			#endif

			/**
			 * Simulation
			 */
			simulate(expanded);

			#ifdef _DEBUG
			simulateTime+=duration_cast<microseconds>(system_clock::now()-temp);
			#endif
		}
	}

	/** Selects the best child node at the given node */
	Node<T,A,E>* select(Node<T,A,E>* node){
		Node<T,A,E>* best=nullptr;
		float bestScore=-std::numeric_limits<float>::max();
		
		std::vector<Node<T,A,E>*>& children=node->getChildren();

		//Select randomly if the Node has not been visited often enough
		if (node->getNumVisits()<minVisits)
			return children[rand()%children.size()];
		
		// Use the UCT formula for selection
		for (Node<T,A,E>* n : children){


			float score=n->getAvgScore()+C*(float)sqrt(log(node->getNumVisits())/n->getNumVisits());

			#ifdef PROG_HIST
			auto stats=history.find(n->getAction());
			if (stats!=history.end()){
				score+=stats->second.second/stats->second.first*W/((1-n->getAvgScore())*n->getNumVisits()+1);
			}
            #endif


			if (score>bestScore)
			{
				bestScore=score;
				best=n;
			}
		}

		return best;
	}
	/** Get the next Action for the given Node, execute and add the new Node to the tree. */
	Node<T,A,E>* expandNext(Node<T,A,E>* node){
		T* expandedData=new T(*node->getData());
		A* action = node->generateNextAction();
		action->execute(expandedData);
		Node<T,A,E>* newNode=new Node<T,A,E>(++currentNodeID, expandedData,node, action);
		node->addChild(newNode);
		return newNode;
	}
	/** Simulate until the stopping condition is reached. */
	void simulate(Node<T,A,E>* node){
		T* state=new T(*node->getData());
		std::vector<Action<T>*> actions;

        A action;
		// Check if the end of the game is reached and generate the next state if not
		while (!termination->isTerminal(state))
		{
			PlayoutStrategy<T,A>* playout=new P(state);
			playout->generateRandom(&action);
			action.execute(state);
			#ifdef PROG_HIST
			actions.push_back(new A(action));
            #endif
			delete playout;
		}

        // Score the leaf node (end of the game)
		float s =scoring->score(state);

		#ifdef PROG_HIST
		// Update progressive history statistics
		for (Action<T>* a : actions){
			auto i = history.find(a);
			if (i!=history.end()){
				std::pair<int, float> stats = i->second;
				stats.first++;
				stats.second+=s;
				i->second=stats;

				delete a;
			}
			else{
				history[a]=std::pair<int, float>(1,s);
			}
		}
        #endif

		backProp(node, s);
		delete state;

	}
	/** Backpropagate a score through the tree */
	void backProp(Node<T,A,E>* node, float score){
		while (node->getParent()!=0){
			node->update(backprop->updateScore(node->getData(),score));
			node=node->getParent();
		}
		node->update(score);
	}
};

#endif
