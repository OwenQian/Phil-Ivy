#ifndef NODE_H
#define NODE_H

#include "../GameObject.h"
#include "../Player.h"

#include <vector>
#include <memory>

class Node: public GameObject {
	protected:
		//static const int EV_Const = 1;
		std::shared_ptr<Node> foldChild;
		std::shared_ptr<Node> callChild;
		std::shared_ptr<Node> raiseChild;
		//the parent of this node in the game tree
		std::shared_ptr<Node> const parent; 
		//number of times a node has been visited/simulated on
		int visitCount;
		//the expected value derived from choosing this node action 
		//updated through backprop
		double expectedValue;
		bool isTerminal;	        //whether or not Node is terminal
		double currentRaise;                   //the current raise

	public:
		// Member-accessibility functions

		// Getters
		std::shared_ptr<Node> getFoldChild() const { return foldChild; }
		std::shared_ptr<Node> getCallChild() const { return callChild; }
		std::shared_ptr<Node> getRaiseChild() const { return raiseChild; }
		//std::shared_ptr<Node> getParent() const { return parent; }
		int getVisitCount() const { return visitCount; }
		double getExpectedValue() const { return expectedValue; }
		bool getTerminalStatus() const { return isTerminal; }
		double getCurrentRaise() const { return currentRaise; }

		// Setters
		void setCurrentRaise(double amount) { currentRaise = amount; }

		// Constructors
		Node(	int                    state,
				double                 pot,
				std::vector<int>       boardCards,
				Player				   botPlayer,
				Player				   oppPlayer,
				int                    playerTurn);

		// Action functions
		virtual std::shared_ptr<Node> fold(); 
		virtual std::shared_ptr<Node> raise(double);
		virtual std::shared_ptr<Node> call(double);
};

#endif	//Node.h
