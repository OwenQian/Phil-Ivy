#include "Node.h"
#include "../Blinds.h"

#include <memory>
#include <algorithm>	//std::min/max
#include <iostream>

Node::Node(int              state,
		double              pot,
		std::vector<int>    boardCards,
		Player				botPlayer,
		Player				oppPlayer,
		int                 playerTurn,
		std::shared_ptr<Node> parent) :
	game(state,
			pot,
			boardCards,
			botPlayer,
			oppPlayer,
			playerTurn),
	parent(parent){ }

	// Action function implementation
