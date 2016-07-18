#ifndef CHOICE_NODE_H
#define CHOICE_NODE_H

#include <memory>

#include "Node.h"
#include "GameObject.h"

class ChoiceNode: public Node {	//inheriting from Node class
	private:
		double currentRaise;
	public:
		// Constructor
		ChoiceNode(std::shared_ptr<Node> const parent, GameObject game);

		

		
};

#endif //Node.h
