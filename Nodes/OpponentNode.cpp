#include "OpponentNode.h"

OpponentNode::OpponentNode(		std::shared_ptr<ChoiceNode>	const parent,
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

OpponentNode::OpponentNode(	std::shared_ptr<OpponentNode>	const parent,
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
