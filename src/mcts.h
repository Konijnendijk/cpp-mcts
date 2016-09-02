#include <vector>
#include <unordered_map>
#include <limits>
#include <cmath>
#include <iostream>
#include <chrono>
#include <cstdlib>

#include "graphviz.h"

#ifndef MCTS_H
#define MCTS_H

using namespace std::chrono;

/**
 * Children of this class should represent game states.
 */
class State{
	friend std::ostream& operator<<(std::ostream& strm, State& s){
		s.print(strm);
	    return strm;
	};

protected:
	virtual void print(std::ostream &strm) {strm << this;};

public:
	virtual ~State(){};
};

/**
 * Children of this class should represent an action a player can execute on a State.
 *
 * Action must implement a copy constructor.
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
	 * Should execute this action on the given state.
	 * @note Cloning the state is not required
	 * @param data
	 */
	virtual void execute(T* data)=0;

	/**
	 * Should return the hash code of this action.
	 * @return
	 */
	virtual size_t hash()=0;

	/**
	 * @return True if this and other are equal, false if not
	 */
	virtual bool equals(Action<T>* other)=0;

	virtual ~Action(){};
};

namespace std{
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

template<class T>
class Strategy{
protected:
	T* state;

public:
	Strategy(T* state) : state(state) {}

	virtual ~Strategy(){};
};

/**
 * Children of this class should generate actions given a state.
 */
template<class T, class A>
class ExpansionStrategy : public Strategy<T> {

public:
	ExpansionStrategy(T* state) : Strategy<T>(state) {}

	/**
	 * Generates the next Action in a sequence. Allows lazy generation.
	 *
	 * @return An Action that has not been returned before, or NULL if no such Action exists
	 */
	virtual A* generateNext()=0;

	/**
	 * @return True if generateNext() can generate a new Action
	 */
	virtual bool canGenerateNext()=0;

	virtual ~ExpansionStrategy() override {}
};

template<class T, class A>
class PlayoutStrategy : public Strategy<T>{

public:
	PlayoutStrategy(T* state) : Strategy<T>(state) {}

	/**
	 * @return A random Action that can be executed on this strategy's State
	 */
	virtual void generateRandom(A* action)=0;

	virtual ~PlayoutStrategy() override {}
};

template<class T>
class Backpropagation  {

public:
	Backpropagation() {}

	/**
	 * @param state the state the score is currently being updated for
	 * @param backpropScore the score being backpropagated
	 * @return An updated score for the current state
	 */
	virtual float updateScore(T* state, float backpropScore)=0;

	virtual ~Backpropagation() {}
};

template<class T>
class TerminationCheck{

public:
	TerminationCheck() {}

	virtual bool isTerminal(T* state)=0;

	virtual ~TerminationCheck(){}
};

template<class T>
class Scoring {

public:
	Scoring() {}

	virtual float score(T* state)=0;

	virtual ~Scoring(){}
};


/**
 * Class used in the internal data structure of MCTS
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
	 * @return This Node's parent or NULL if no parent exists (this Node is the root)
	 */
	Node<T,A,E>* getParent(){
		return parent;
	}

	/**
	 * @return All children of this Node
	 */
	std::vector<Node<T,A,E>*> getChildren(){
		return children;
	}

	/**
	 * @return The Action to execute on the parent's State to get from the parent's State to this Node's State.
	 */
	A* getAction(){
		return action;
	}

	/**
	 * @return A new action if there are any remaining, NULL if not
	 */
	A* generateNextAction(){
		return expansion->generateNext();
	}

	/**
	 * Add a child to this Node's children
	 * @param child The child to add
	 */
	void addChild(Node<T,A,E>* child){
		children.push_back(child);
	}

	/**
	 * Checks this Node's ActionGenerator if there are more Actions to be generated.
	 * @return True if it is still possible to add children
	 */
	bool shouldExpand(){
		bool result=children.empty() || expansion->canGenerateNext();
		return result;
	}

	/**
	 * Update this Node's score and increment the number of visits.
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
 * AI search technique for finding the best Action give a certain State.
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

	/** Minimum number of visits until a Node will be selected using the UCT formula, below this number random selection is used */
	const int MIN_VISITS=2;

	/** Minimum number of visits until a Node will be expanded */
	const int DEFAULT_MIN_T=5;

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

	/** Variable to assign IDs to a node */
	unsigned int currentNodeID;

	/*
	 * Debugging variables
	 */
	microseconds selectTime, expandTime,simulateTime;
	long iterations;

public:
	/**
	 * backprop, termination and scoring will be deleted by this instance.
	 */
	MCTS(T* rootData, Backpropagation<T>* backprop, TerminationCheck<T>* termination, Scoring<T>* scoring) : backprop(backprop), termination(termination), scoring(scoring), root(new Node<T,A,E>(0, rootData, 0, new A())), history(), time(milliseconds(DEFAULT_TIME)), minIterations(DEFAULT_MIN_ITERATIONS), C(DEFAULT_C), W(DEFAULT_W), minT(DEFAULT_MIN_T), currentNodeID(0), selectTime(microseconds::zero()), expandTime(microseconds::zero()),simulateTime(microseconds::zero()), iterations(0) {}

	/**
	 * Runs the MCTS algorithm and searches for the best Action
	 * @return The Action found by MCTS
	 */
	A* calculateAction(){

		system_clock::time_point old=system_clock::now();

		search();

		std::cerr << iterations << " iterations in " << duration_cast<milliseconds>(system_clock::now()-old).count() << "ms" << std::endl;

		#ifdef _DEBUG
		std::cerr << "Average select:" << (float)(selectTime.count()/1000)/iterations/1000 << "ms Average expand:" << (float)(expandTime.count())/iterations/1000 << "ms Average simulate:" << (float)(simulateTime.count())/iterations/1000 << "ms" << std::endl;
		#endif

		// Select the Action with the best score
		Node<T,A,E>* best=NULL;
		float bestScore=-std::numeric_limits<float>::max();
		std::vector<Node<T,A,E>*> children=root->getChildren();

		for (unsigned int i=0; i<children.size();i++){
			float score=children[i]->getAvgScore();
			if (score>bestScore){
				bestScore=score;
				best=children[i];
			}
		}

		#ifdef _DEBUG
		writeDotFile(root, "mcts.dot");
        #endif

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
	 * Set the C parameter of the UCT formula
	 * @param C
	 */
	void setC(float C){
		this->C=C;
	}

	void setW(float W){
		this->W=W;
	}

	void setMinT(float minT){
		this->minT = minT;
	}

	/**
	 * Get the root of the MCTS tree. Useful for printing.
	 * @return
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
		Node<T,A,E>* best=NULL;
		float bestScore=-std::numeric_limits<float>::max();
		
		std::vector<Node<T,A,E>*> children=node->getChildren();

		//Select randomly if the Node has not been visited often enough
		if (node->getNumVisits()<MIN_VISITS)
			return children[rand()%children.size()];
		
		// Use the UCT formula for selection
		for (Node<T,A,E>* n : children){


			float score=n->getAvgScore()+C*sqrt(log(node->getNumVisits())/n->getNumVisits());

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
	/** Get the next Action for the given Node, execute if and add the new Node to the tree. */
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
