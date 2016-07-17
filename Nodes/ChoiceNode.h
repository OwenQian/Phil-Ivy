#ifndef CHOICE_NODE_H
#define CHOICE_NODE_H

#include <memory>

#include "Node.h"
#include "GameObject.h"

class ChoiceNode: public Node {	//inheriting from Node class
	private:
		double invested;
	public:
		// Constructor
		ChoiceNode(std::shared_ptr<Node> const parent, GameObject game);

		// Action functions
		std::shared_ptr<Node> fold();
		std::shared_ptr<Node> raise(double raiseAmount);
		std::shared_ptr<Node> check();

		std::shared_ptr<Node> fold() {
			std::shared_ptr<Node> foldNode = new Node();
			foldNode->isTerminal = true;
			this->childList[0] = foldNode;
			return foldNode;
		}

		std::shared_ptr<Node> raise(double raiseAmount) {
			std::shared_ptr<Node> raiseNode = new Node(int state,
					double (pot + raiseAmount),
					std::vector<int> boardCards,
					std::vector<Player> playerList,
					int ++playerTurn);
		}
};

#endif //Node.h
