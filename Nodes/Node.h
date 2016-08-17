#ifndef NODE_H
#define NODE_H

#include "../GameObject.h"
#include "../Player.h"

#include <vector>
#include <memory>

class ChoiceNode;
class OpponentNode;

class Node: public std::enable_shared_from_this<Node> {
	protected:
		std::shared_ptr<Node> foldChild;
		std::shared_ptr<Node> callChild;
		std::shared_ptr<Node> raiseChild;
		GameObject game;
		//the parent of this node in the game tree
		std::shared_ptr<Node> const parent; 
		//number of times a node has been visited/simulated on
		//the expected value derived from choosing this node action 
		//updated through backprop
		double currentRaise;                   //the current raise
		bool isFolded = false;	        //whether or not Node is Folded
		bool isAllIn = false;				//only true when accepted
		bool firstAction;
        int visitCount = 0;
        double expectedValue = 0.0;
        static double const exploreConst;

		// Action functions implementations
		// Note, if we're not using different implementations of these functions for c-o/node
		// these don't need to be virtual, but they do if we are
		std::shared_ptr<Node> doFold();
		std::shared_ptr<Node> doRaise(double);
		std::shared_ptr<Node> doCall();
		std::shared_ptr<Node> doTerminal();

     	public:
		// Member-accessibility functions

		// Getters
		GameObject & getGame() { return game; }
		GameObject const & getGame() const { return game; }
		std::shared_ptr<Node> getFoldChild() const { return foldChild; }
		std::shared_ptr<Node> getCallChild() const { return callChild; }
		std::shared_ptr<Node> getRaiseChild() const { return raiseChild; }
		std::shared_ptr<Node> getParent() const { return parent; }
		bool getIsFolded() const { return isFolded; }
		double const & getCurrentRaise() const { return currentRaise; }
		bool getIsAllIn() const { return isAllIn; }
		bool getIsFirst() const { return firstAction; }
        int & getVisitCount() { return visitCount; }
        int const & getVisitCount() const { return visitCount; }
        double & getExpectedValue() { return expectedValue; }
        double const & getExpectedValue() const { return expectedValue; }

		// Setters
		void setCurrentRaise(double amount) { currentRaise = amount; }
		void setIsAllIn(bool status) { isAllIn = status; }
		void setIsFirst(bool a) { firstAction = a; }
        void incrementVisitCount() { ++visitCount; }
        void setExpectedValue(double EV) { expectedValue = EV; }
		
		// Constructors
		Node(	int                    state,
				double                 pot,
				std::vector<int>       boardCards,
				Player				   botPlayer,
				Player				   oppPlayer,
				int                    playerTurn,
				std::shared_ptr<Node>  parent);

};

const double Node::exploreConst = 1.0;

#endif	//Node.h
