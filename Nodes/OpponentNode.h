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
		std::shared_ptr<ChoiceNode> doFold() {
			return std::static_pointer_cast<ChoiceNode> (fold());
		}
		std::shared_ptr<ChoiceNode> doCall(double callAmount) {
			return std::static_pointer_cast<ChoiceNode> (call(callAmount));
		}
		std::shared_ptr<ChoiceNode> doRaise(double raiseAmount) {
			return std::static_pointer_cast<ChoiceNode> (raise(raiseAmount));
		}
};

#endif	//OpponentNode.h
