#ifndef CHOICE_NODE_H
#define CHOICE_NODE_H

#include "Node.h"
#include "../GameObject.h"

#include <memory>		//smart pointer

class OpponentNode;

class ChoiceNode: public Node {	//inheriting from Node class
	private:

	public:
		// Constructor
		ChoiceNode(
				int								state,
				double							pot,
				std::vector<int>				boardCards,
				Player							botPlayer,
				Player							oppPlayer,
				int								playerTurn,
				std::shared_ptr<Node>		const parent);

		OpponentNode& fold();
		OpponentNode& raise(double);
		OpponentNode& call();
};

#endif //Node.h
