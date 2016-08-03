#include "ChoiceNode.h"
#include "OpponentNode.h"

ChoiceNode::ChoiceNode(		std::shared_ptr<ChoiceNode>	const parent,
							int							state,
							double						pot,
							std::vector<int>			boardCards,
							Player						botPlayer,
							Player						oppPlayer,
							int							playerTurn) :
	Node(	state,
			pot,
			boardCards,
			botPlayer,
			oppPlayer,
			playerTurn),
	parent(parent) { }

ChoiceNode::ChoiceNode(		std::shared_ptr<OpponentNode>	const parent,
							int								state,
							double							pot,
							std::vector<int>				boardCards,
							Player							botPlayer,
							Player							oppPlayer,
							int								playerTurn) :
	Node(	state,
			pot,
			boardCards,
			botPlayer,
			oppPlayer,
			playerTurn),
	parent(parent) { }
