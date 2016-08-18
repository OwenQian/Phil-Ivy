#ifndef OPPONENT_NODE_H
#define OPPONENT_NODE_H

#include "Node.h"
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
						std::shared_ptr<Node>	const parent);
		
		// Action functions
		ChoiceNode& fold();
		ChoiceNode& raise(double);
		ChoiceNode& call();
};

#endif	//OpponentNode.h
