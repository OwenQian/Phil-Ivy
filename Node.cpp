#include "Node.h"

#include <vector>

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
