#ifndef OPPONENT_NODE_H
#define OPPONENT_NODE_H

#include "Node.h"
#include "Player.h"

class OpponentNode: public Node {
	private:
		double invested;
	public:
		//Constructor
		OpponentNode(Node* const parent, GameObject game);

		//Action functions
		Node fold();
		Node raise(double raiseAmount);
		Node check();
};

OpponentNode::OpponentNode(Node* const parent,
		GameObject game):
	parent(parent), game(game) 
{
	visitCount = 0;
	isTerminal = false;
}

#endif	//OpponentNode.h
