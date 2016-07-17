#ifndef CHOICE_NODE_H
#define CHOICE_NODE_H

#include "Node.h"
#include "GameObject.h"

class ChoiceNode: public Node {	//inheriting from Node class
	private:
		double invested;
	public:
		// Constructor
		ChoiceNode(Node* const parent, GameObject game);

		// Action functions
		*Node fold();
		*Node raise(double raiseAmount);
		*Node check();

		*Node fold() {
			*Node foldNode = new Node();
			foldNode->isTerminal = true;
			this->childList[0] = foldNode;
			return foldNode;
		}

		*Node raise(double raiseAmount) {
			*Node raiseNode = new Node(int state,
					double (pot + raiseAmount),
					std::vector<int> boardCards,
					std::vector<Player> playerList,
					int ++playerTurn);
		}
};

#endif //Node.h
