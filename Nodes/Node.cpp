#include "Node.h"

#include <memory>

Node::Node(int              state,
		double              pot,
		std::vector<int>    boardCards,
		Player				botPlayer,
		Player				oppPlayer,
		int                 playerTurn,
		std::shared_ptr<Node> parent) :
	GameObject(state,
			pot,
			boardCards,
			botPlayer,
			oppPlayer,
			playerTurn),
	parent(parent) { }

	// Action function implementation
	std::shared_ptr<Node> Node::fold() {
		//create child foldNode
		std::shared_ptr<Node> foldNode(new Node(*this));
		foldNode->isTerminal = true;
		foldChild = foldNode;
		return foldNode;
	}

std::shared_ptr<Node> Node::call(double callAmount) { //remove call amount later
	//creates a temporary playerlist and updates the player's potinvestment and chip count
	if (getPlayerTurn() == 0){
		Player tempPlayer = getBotPlayer();
		tempPlayer.setPotInvestment(callAmount + tempPlayer.getPotInvestment());
		tempPlayer.setChips(tempPlayer.getChips() - callAmount);
		auto callNode = std::make_shared<Node>( getState() + 1,
			getPot() + callAmount,
			getBoardCards(),
			tempPlayer,
			getOppPlayer(),
			getPlayerTurn() + 1,
			std::shared_ptr<Node> (this) );
		callChild = callNode;
	} else {
		Player tempPlayer = getOppPlayer();
		tempPlayer.setPotInvestment(callAmount + tempPlayer.getPotInvestment());
		tempPlayer.setChips(tempPlayer.getChips() - callAmount);
		auto callNode = std::make_shared<Node>( getState() + 1,
			getPot() + callAmount,
			getBoardCards(),
			getBotPlayer(),
			tempPlayer,
			getPlayerTurn() + 1,
			std::shared_ptr<Node> (this) );
		callChild = callNode;
	}
	return callChild;
}

std::shared_ptr<Node> Node::raise(double raiseAmount) {
	if (getPlayerTurn() == 0){
		Player tempPlayer = getBotPlayer();
		tempPlayer.setPotInvestment(raiseAmount + tempPlayer.getPotInvestment());
		tempPlayer.setChips(tempPlayer.getChips() - raiseAmount);
		auto raiseNode = std::make_shared<Node>( getState(),
			getPot() + raiseAmount,
			getBoardCards(),
			tempPlayer,
			getOppPlayer(),
			getPlayerTurn() + 1,
			std::shared_ptr<Node> (this) );
		(*raiseNode).addCurrentRaise(raiseAmount);
		raiseChild = raiseNode;
	} else {
		Player tempPlayer = getOppPlayer();
		tempPlayer.setPotInvestment(raiseAmount + tempPlayer.getPotInvestment());
		tempPlayer.setChips(tempPlayer.getChips() - raiseAmount);
		auto raiseNode = std::make_shared<Node>( getState(),
			getPot() + raiseAmount,
			getBoardCards(),
			getBotPlayer(),
			tempPlayer,
			getPlayerTurn() + 1,
			std::shared_ptr<Node> (this) );
		(*raiseNode).addCurrentRaise(raiseAmount);
		raiseChild = raiseNode;
	}
	
	return raiseChild;
}
