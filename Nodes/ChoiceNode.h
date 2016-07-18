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

// ##################### Start of Implementation ##########################

		std::shared_ptr<Node> fold() {
			std::shared_ptr<Node> foldNode(new Node() );   //create child foldNode
			foldNode->isTerminal = true;
			foldNode->getPlayerList()[1].setIsFolded(true);
			this->childList[0] = foldNode;                 //childList[0] holds the foldNodes
			return foldNode;
		}

//TODO make sure this function is correct
		std::shared_ptr<Node> call() {
			GameObject checkGame(game.getState() + 1,
					game.getPot(),
					game.getSmallBlind(),
					game.getBigBlind(),
					game.getBoardCards(),
					game.getPlayerList());
			ChoiceNode* choiceCheck = new ChoiceNode(this, checkGame);
			return choiceCheck;
		}
};

#endif //Node.h
