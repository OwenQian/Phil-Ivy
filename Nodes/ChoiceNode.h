#ifndef CHOICE_NODE_H
#define CHOICE_NODE_H

#include <memory>
#include <algorithm>
#include <vector>
#include <iterator>

#include "Node.h"
#include "GameObject.h"

class ChoiceNode: public Node {	//inheriting from Node class
	
	public:
		// Constructor
		ChoiceNode(std::shared_ptr<Node> const parent, GameObject game);

// ##################### Start of Implementation ##########################
		std::shared_ptr<Node> fold();
		std::shared_ptr<Node> call();
		std::shared_ptr<Node> raise();
};
		std::shared_ptr<Node> fold() {
			std::shared_ptr<Node> foldNode(new Node() );   //create child foldNode
			foldNode->isTerminal = true;
			foldNode->getPlayerList()[1].setIsFolded(true);
			this->childList[0] = foldNode;                 //childList[0] holds the foldNodes
			return foldNode;
		}

//TODO make sure this function is correct
		std::shared_ptr<Node> call() {
			//creates a temporary playerlist and updates the player's potinvestment and chip count
			std::vector<Player>tempPlayerList;
			tempPlayerList.reserve(game.getPlayerList().size());
			//copying the elements of current playerlist into the temp
			std::copy(game.getPlayerList().begin(), game.getPlayerList().end(), back_inserter(tempPlayerList));
			//updating the values
			tempPlayerList[getPlayerTurn()].setChips(tempPlayerList[getPlayerTurn()].getChips() - getCurrentRaise()); // NOT STRAIGHT
			tempPlayerList[getPlayerTurn()].setPotInvestment(tempPlayerList[getPlayerTurn()].getPotInvestment() + getCurrentRaise()); //GAY
			
			Node checkGame(getState() + 1,
					getPot() + getCurrentRaise(),
					getSmallBlind(),
					getBigBlind(),
					getBoardCards(),
					tempPlayerList);
			
			ChoiceNode* choiceCheck = new ChoiceNode(this, checkGame);
			return choiceCheck;
		}
		
		std::shared_ptr<Node> Node::raise(double amount) {
			
		}

#endif //Node.h
