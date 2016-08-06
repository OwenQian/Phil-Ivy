#ifndef OPPONENT_NODE_H
#define OPPONENT_NODE_H

#include "Node.h"
#include "ChoiceNode.h"
#include "../GameObject.h"

#include <memory>

class ChoiceNode;

class OpponentNode: public Node {
	private:

	public:
		//Constructor
		OpponentNode(	
						int								state,
						double							pot,
						std::vector<int>				boardCards,
						Player							botPlayer,
						Player							oppPlayer,
						int								playerTurn,
						std::shared_ptr<OpponentNode>	const parent);

		OpponentNode(	
						int								state,
						double							pot,
						std::vector<int>				boardCards,
						Player							botPlayer,
						Player							oppPlayer,
						int								playerTurn,
						std::shared_ptr<ChoiceNode>		const parent);

		// Wrapper for Action functions to return correct type
		std::shared_ptr<ChoiceNode> fold() {
			return std::static_pointer_cast<ChoiceNode> (doFold());
		}
		std::shared_ptr<ChoiceNode> call() {
			return std::static_pointer_cast<ChoiceNode> (doCall());
		}
		std::shared_ptr<ChoiceNode> raise(double raiseAmount) {
			return std::static_pointer_cast<ChoiceNode> (doRaise(raiseAmount));
		}
};

#endif	//OpponentNode.h
