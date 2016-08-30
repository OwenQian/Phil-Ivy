#ifndef CHOICE_NODE_H
#define CHOICE_NODE_H

#include "Node.h"
#include "../GameObject.h"
#include "../GameUtilities/GameUtilities.h"
#include "../Stage.h"
#include "../GameUtilities/Action.h"

#include <memory>		//smart pointer

class ChoiceNode: public Node {	//inheriting from Node class
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

#endif //Node.h
