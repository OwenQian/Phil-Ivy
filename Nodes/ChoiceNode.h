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
        virtual std::unique_ptr<Node>& call();
        virtual std::unique_ptr<Node>& raise(double);
        virtual std::unique_ptr<Node>& fold();

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
