#ifndef CHOICE_NODE_H
#define CHOICE_NODE_H

#include <memory>		//smart pointer

#include "Node.h"
#include "../Action.h"
#include "../GameObject.h"

class ChoiceNode: public Node {	//inheriting from Node class
    private: 
        virtual Decision makeDecision();
    public:
        virtual void call();
        virtual void raise(double);
        virtual void fold();

		// Constructor
        ChoiceNode();
		ChoiceNode(
				int								state,
				double							pot,
				std::vector<int>				boardCards,
				Player							botPlayer,
				Player							oppPlayer,
				int								playerTurn,
				Node*	                        const parent);
        ChoiceNode(const ChoiceNode&);
};

#endif //ChoiceNode.h
