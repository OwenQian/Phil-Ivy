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
		bool isTerminal;

	public:
		std::vector<Node> getChildList();
		Node* getParent();
		GameInfo getGame();
		int getVisitCount();
		double getExpectedValue();
		bool getTerminalStatus();
		static virtual Node fold(); 
		static virtual Node raise();
		static virtual Node check();
};

#endif	//Node.h
