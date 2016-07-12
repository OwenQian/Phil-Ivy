#ifndef CHOICE_NODE_H
#define CHOICE_NODE_H

#include "Node.h"
#include "GameInfo.h"

class ChoiceNode: public Node {	//inheriting from Node class
	private:
		double invested;
	public:
		// Constructor
		ChoiceNode(Node* const parent, GameInfo game);

		// Action functions
		*Node fold();
		*Node raise(double raiseAmount);
		*Node check();
};

#endif //Node.h
