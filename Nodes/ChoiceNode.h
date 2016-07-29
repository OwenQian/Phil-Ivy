#ifndef CHOICE_NODE_H
#define CHOICE_NODE_H

#include "Node.h"
#include "../GameObject.h"

#include <memory>		//smart pointer

class ChoiceNode: public Node {	//inheriting from Node class
	private:

	public:
		// Constructor
		ChoiceNode(		std::shared_ptr<OpponentNode>	const parent,
						int								state,
						double							pot,
						std::vector<int>				boardCards,
						Player							botPlayer,
						Player							oppPlayer,
						int								playerTurn);

		ChoiceNode(		std::shared_ptr<ChoiceNode>		const parent,
						int								state,
						double							pot,
						std::vector<int>				boardCards,
						Player							botPlayer,
						Player							oppPlayer,
						int								playerTurn);

		// Wrapper for action functions to return correct type
		std::shared_ptr<OpponentNode> doFold() {
			return std::static_pointer_cast<OpponentNode> (fold());
		}
		std::shared_ptr<OpponentNode> doCall() {
			return std::static_pointer_cast<OpponentNode> (call());
		}
		std::shared_ptr<OpponentNode> doRaise() {
			return std::static_pointer_cast<OpponentNode> (raise());
		}
};

#endif //Node.h
