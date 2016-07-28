#ifndef CHOICE_NODE_H
#define CHOICE_NODE_H

#include "Node.h"
#include "../GameObject.h"

#include <memory>		//smart pointer
#include <vector>

class ChoiceNode: public Node {	//inheriting from Node class
	private:
		//Made private because they're called by doFold, doCall and doRaise only

	public:
		// Constructor
		ChoiceNode(std::shared_ptr<Node> const parent, GameObject game);

		// Wrapper for action functions to return correct type
		std::shared_ptr<OpponentNode> doFold() {
			return std::static_pointer_cast<OpponentNode> (fold());
		}
		std::shared_ptr<OpponentNode> doCall() {
			return std::static_pointer_cast<OpponentNode> (call());
		}
		std::shared_ptr<OpponentNode> doRaise() {
			return std::static_pointer_cast<OpponentNode> (raise());
		}
};

#endif //Node.h
