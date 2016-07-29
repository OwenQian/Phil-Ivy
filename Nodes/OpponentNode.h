#ifndef OPPONENT_NODE_H
#define OPPONENT_NODE_H

#include "Node.h"
#include "../GameObject.h"

#include <memory>

class OpponentNode: public Node {
	private:

	public:
		//Constructor
		OpponentNode(	std::shared_ptr<OpponentNode>	const parent,
						int								state,
						double							pot,
						std::vector<int>				boardCards,
						Player							botPlayer,
						Player							oppPlayer,
						int								playerTurn);

		OpponentNode(	std::shared_ptr<ChoiceNode>		const parent,
						int								state,
						double							pot,
						std::vector<int>				boardCards,
						Player							botPlayer,
						Player							oppPlayer,
						int								playerTurn);

		// Wrapper for Action functions to return correct type
		std::shared_ptr<ChoiceNode> doFold() {
			return std::static_pointer_cast<ChoiceNode> (fold());
		}
		std::shared_ptr<ChoiceNode> doCall() {
			return std::static_pointer_cast<ChoiceNode> (call());
		}
		std::shared_ptr<ChoiceNode> doRaise() {
			return std::static_pointer_cast<ChoiceNode> (raise());
		}
};

#endif	//OpponentNode.h
