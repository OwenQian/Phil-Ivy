#ifndef NODE_H
#define NODE_H

#include "GameInfo.h"
#include "Player.h"

#include <vector>

class Node {
	protected:
		static const int EV_Const = 1;
		std::vector<Node> childList;
		Node* const parent;
		GameInfo game;
		int visitCount;
		double expectedValue;
		bool isTerminal = false;	//whether or note Node is terminal
		double callAmount - 0;

	public:
		//member-accessibility functions
		std::vector<Node> getChildList() const;
		Node* getParent() const;
		GameInfo getGame() const;
		int getVisitCount() const;
		double getExpectedValue() const;
		bool getTerminalStatus() const;

		//functions to be implemented differently for ChoiceNode and \
		//OpponentNode
		virtual Node fold() = 0; 
		virtual Node raise(double raiseAmount) = 0;
		virtual Node check() = 0;
};

	std::vector<Node> Node::getChildList() {
		return childList;
	}

	Node* Node::getParent() {
		return parent;
	}

	GameInfo Node::getGame() {
		return game;
	}

	int Node::getVisitCount() {
		return visitCount;
	}

	double Node::getExpectedValue() {
		return expectedValue;
	}

	bool Node::getTerminalStatus() {
		return isTerminal;
	}
#endif	//Node.h
