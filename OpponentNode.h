#ifndef OPPONENT_NODE_H
#define OPPONENT_NODE_H

#include "Node.h"

class OpponentNode: public Node {
	protected:
	public:
		//Constructor
		OpponentNode();

		//Action functions
		static Node fold();
		static Node raise(double raiseAmount);
		static Node check();
};

#endif	//OpponentNode.h
