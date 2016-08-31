#ifndef OPPONENT_NODE_H
#define OPPONENT_NODE_H

#include <memory>

#include "Node.h"
#include "../GameObject.h"

class ChoiceNode;
class OpponentNode: public Node {
    public:
        virtual std::unique_ptr<Node>& call();
        virtual std::unique_ptr<Node>& raise(double);
        virtual std::unique_ptr<Node>& fold();

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
