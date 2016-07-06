#ifndef NODE_H
#define NODE_H

#include <vector>

class Node {
	protected:
		std::vector<Node> childList;
		Node* const parent;
		GameInfo game;
		int visitCount;
		double expectedValue;
		bool isTerminal;	//whether or note Node is terminal

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
		static virtual Node fold() = 0; 
		static virtual Node raise(double raiseAmount) = 0;
		static virtual Node check() = 0;
};

#endif	//Node.h
