#ifndef CHOICE_NODE_H
#define CHOICE_NODE_H

#include "Node.h"

class ChoiceNode: public Node {	//inheriting from Node class
	protected:
	public:
		// Constructor
		ChoiceNode();

		// Action functions
		static Node fold();
		static Node raise(double raiseAmount);
		static Node check();
};

#endif	//ChoiceNode.h
