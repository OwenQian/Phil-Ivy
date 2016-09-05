#ifndef OPPONENT_NODE_H
#define OPPONENT_NODE_H

#include <memory>

#include "Node.h"
#include "../GameObject.h"
#include "../Action.h"
#include "../GameUtilities/Decision.h"

class ChoiceNode;
class OpponentNode: public Node {
    private:
        virtual Decision makeDecision();
    public:
        virtual void call();
        virtual void raise(double);
        virtual void fold();

        // Contructors
        OpponentNode();
		OpponentNode(	
						int								state,
						double							pot,
						std::vector<int>				boardCards,
						Player							botPlayer,
						Player							oppPlayer,
						int								playerTurn,
						Node*	                    	const parent);
        OpponentNode(const OpponentNode&);
};

#endif	//OpponentNode.h
