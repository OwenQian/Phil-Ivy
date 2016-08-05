#ifndef NODE_H
#define NODE_H

#include "../GameObject.h"
#include "../Player.h"

#include <vector>
#include <memory>

class Node {
	protected:
		//static const int EV_Const = 1
		GameObject game;
		std::shared_ptr<Node> foldChild;
		std::shared_ptr<Node> callChild;
		std::shared_ptr<Node> raiseChild;
		//the parent of this node in the game tree
		std::shared_ptr<Node> const parent; 
		//number of times a node has been visited/simulated on
		//the expected value derived from choosing this node action 
		//updated through backprop
		double currentRaise;                   //the current raise
		bool isTerminal;	        //whether or not Node is terminal
		bool isAllIn;				//only true when accepted

	public:
		// Member-accessibility functions

		// Getters
		GameObject getGame() const { return game; }
		std::shared_ptr<Node> getFoldChild() const { return foldChild; }
		std::shared_ptr<Node> getCallChild() const { return callChild; }
		std::shared_ptr<Node> getRaiseChild() const { return raiseChild; }
		//std::shared_ptr<Node> getParent() const { return parent; }
		bool getTerminalStatus() const { return isTerminal; }
		double getCurrentRaise() const { return currentRaise; }
		bool getIsAllIn() const { return isAllIn; }

		// Setters
		void setCurrentRaise(double amount) { currentRaise = amount; }
		void setIsAllIn(bool status) { isAllIn = status; }
		
		// Constructors
		Node(	int                    state,
				double                 pot,
				std::vector<int>       boardCards,
				Player				   botPlayer,
				Player				   oppPlayer,
				int                    playerTurn,
				std::shared_ptr<Node>  parent);

		// Action functions
		virtual std::shared_ptr<Node> fold(); 
		virtual std::shared_ptr<Node> raise(double);
		virtual std::shared_ptr<Node> call();
};

#endif	//Node.h
