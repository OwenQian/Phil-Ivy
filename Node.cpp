#include "Node.h"

#include <vector>

class Node {
	isTerminal = false;

	std::vector<Node> getChildList() {
		return childList;
	}

	Node* getParent() {
		return parent;
	}

	GameInfo getGame() {
		return game;
	}

	int getVisitCount() {
		return visitCount;
	}

	double getExpectedValue() {
		return expectedValue;
	}

	bool getTerminalStatus() {
		return isTerminal;
	}
};
