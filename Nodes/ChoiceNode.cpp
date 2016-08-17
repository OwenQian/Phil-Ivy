#include "ChoiceNode.h"
#include "OpponentNode.h"
#include "../Stage.h"

#include <memory>
#include <chrono>

ChoiceNode::ChoiceNode(
							int							state,
							double						pot,
							std::vector<int>			boardCards,
							Player						botPlayer,
							Player						oppPlayer,
							int							playerTurn,
							std::shared_ptr<ChoiceNode>	const parent) :
	Node(	state,
			pot,
			boardCards,
			botPlayer,
			oppPlayer,
			playerTurn,
			parent) { }

ChoiceNode::ChoiceNode(
							int								state,
							double							pot,
							std::vector<int>				boardCards,
							Player							botPlayer,
							Player							oppPlayer,
							int								playerTurn,
							std::shared_ptr<OpponentNode>	const parent) :
	Node(	state,
			pot,
			boardCards,
			botPlayer,
			oppPlayer,
			playerTurn,
			parent) { }
    
void ChoiceNode::backPropagate(ChoiceNode& nextNode, double botEV, double oppEV) {
    nextNode.getExpectedValue() = (nextNode.getExpectedValue() * nextNode.getVisitCount() + botEV) / ++(nextNode.getVisitCount());
    if (nextNode.getParent() != nullptr)
        ChoiceNode::backPropagate(*(nextNode.getParent()), nextNode.getExpectedValue(), oppEV);
}

void ChoiceNode::backPropagate(OpponentNode& nextNode, double botEV, double oppEV) {
    nextNode.getExpectedValue() = (nextNode.getExpectedValue() * nextNode.getVisitCount() + oppEV) / ++(nextNode.getVisitCount());
    if (nextNode.getParent() != nullptr)
        backPropagate(*(nextNode.getParent()), botEV, nextNode.getExpectedValue());
}

Action monteCarlo(int maxSeconds) {
    
}
