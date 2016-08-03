#ifndef CHOICE_NODE_H
#define CHOICE_NODE_H

#include "Node.h"
#include "OpponentNode.h"
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
				std::shared_ptr<OpponentNode>	const parent);

		ChoiceNode(
				int								state,
				double							pot,
				std::vector<int>				boardCards,
				Player							botPlayer,
				Player							oppPlayer,
				int								playerTurn,
				std::shared_ptr<ChoiceNode>		const parent);

		// Wrapper for action functions to return correct type
		std::shared_ptr<OpponentNode> doFold() {
			return std::static_pointer_cast<OpponentNode> (fold());
		}
		std::shared_ptr<OpponentNode> doCall(double callAmount) {
			return std::static_pointer_cast<OpponentNode> (call(callAmount));
		}
		std::shared_ptr<OpponentNode> doRaise(double raiseAmount) {
			return std::static_pointer_cast<OpponentNode> (raise(raiseAmount));
		}
};

#endif //Node.h
