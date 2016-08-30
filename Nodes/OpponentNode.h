#ifndef OPPONENT_NODE_H
#define OPPONENT_NODE_H

#include "Node.h"
#include "../GameObject.h"
#include "../GameUtilities/GameUtilities.h"
#include "../Stage.h"
#include "../GameUtilities/Action.h"

#include <memory>

class ChoiceNode;
class OpponentNode: public Node {
    public:
		OpponentNode(	
						int								state,
						double							pot,
						std::vector<int>				boardCards,
						Player							botPlayer,
						Player							oppPlayer,
						int								playerTurn,
						Node*	                    	const parent);

        virtual std::unique_ptr<Node>& call();
        virtual std::unique_ptr<Node>& raise(double);
        virtual std::unique_ptr<Node>& fold();
};

#endif	//OpponentNode.h
