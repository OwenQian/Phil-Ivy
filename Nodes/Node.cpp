#include "Node.h"

#include <memory>
#include <iostream>

Node::Node(int              state,
		double              pot,
		std::vector<int>    boardCards,
		Player				botPlayer,
		Player				oppPlayer,
		int                 playerTurn,
		std::shared_ptr<Node> parent) :
	game(state,
			pot,
			boardCards,
			botPlayer,
			oppPlayer,
			playerTurn),
	parent(parent),
	isFolded(false),
	isAllIn(false) { }

	// Action function implementation
	std::shared_ptr<Node> Node::doFold() {
		//create child foldNode
		auto foldNode = std::make_shared<Node>(*this);
		foldNode->isFolded = true;
		foldChild = foldNode;
		return foldNode;
	}

std::shared_ptr<Node> Node::doCall() {
	//creates a temporary playerlist and updates the player's potinvestment and chip count
	if (game.getPlayerTurn() == 0) {
		Player tempPlayer = game.getBotPlayer();
		tempPlayer.setPotInvestment(currentRaise + tempPlayer.getPotInvestment());
		tempPlayer.setChips(tempPlayer.getChips() - currentRaise);
		bool tempAllIn = false;
		if (game.getBotPlayer().getChips() == currentRaise ||
				game.getOppPlayer().getChips() == currentRaise) {
			tempAllIn = true;
		}
		auto callNode = std::make_shared<Node>(game.getState() + 1,
			game.getPot() + currentRaise,
			game.getBoardCards(),
			tempPlayer,
			game.getOppPlayer(),
			!(game.getPlayerTurn()),
			std::make_shared<Node> (*this) );
		(*callNode).setIsAllIn(tempAllIn);
		callChild = callNode;
	} else {
		Player tempPlayer = game.getOppPlayer();
		tempPlayer.setPotInvestment(currentRaise + tempPlayer.getPotInvestment());
		tempPlayer.setChips(tempPlayer.getChips() - currentRaise);
		bool tempAllIn = false;
		if (game.getBotPlayer().getChips() == currentRaise ||
				game.getOppPlayer().getChips() == currentRaise) {
			tempAllIn = true;
		}
		auto callNode = std::make_shared<Node>( game.getState() + 1,
			game.getPot() + currentRaise,
			game.getBoardCards(),
			game.getBotPlayer(),
			tempPlayer,
			!(game.getPlayerTurn()),
			std::make_shared<Node> (*this) );
		(*callNode).setIsAllIn(tempAllIn);
		callChild = callNode;
	}
	return callChild;
}

// raiseAmount means amount raising to, NOT raising by
std::shared_ptr<Node> Node::doRaise(double raiseAmount) {
	// if raise all-in (or more) create AllInNode
	if (raiseAmount >= game.getBotPlayer().getChips() ||
			raiseAmount >= game.getOppPlayer().getChips() ) {
		std::cout << "Raising All-In" << std::endl;
		// set raiseAmount to lesser of chip amounts
		raiseAmount = (game.getBotPlayer().getChips() < game.getOppPlayer().getChips()) ? 
			game.getBotPlayer().getChips() : game.getOppPlayer().getChips();
	}
	if (game.getPlayerTurn() == 0) {
		Player tempPlayer = game.getBotPlayer();
		tempPlayer.setPotInvestment(raiseAmount + tempPlayer.getPotInvestment());
		tempPlayer.setChips(tempPlayer.getChips() - raiseAmount);
		auto raiseNode = std::make_shared<Node>( game.getState(),
			game.getPot() + raiseAmount,
			game.getBoardCards(),
			tempPlayer,
			game.getOppPlayer(),
			!(game.getPlayerTurn()),
			std::make_shared<Node> (*this) );
		(*raiseNode).setCurrentRaise(raiseAmount);
		raiseChild = raiseNode;
	} else {
		Player tempPlayer = game.getOppPlayer();
		tempPlayer.setPotInvestment(raiseAmount + tempPlayer.getPotInvestment());
		tempPlayer.setChips(tempPlayer.getChips() - raiseAmount);
		auto raiseNode = std::make_shared<Node>( game.getState(),
			game.getPot() + raiseAmount,
			game.getBoardCards(),
			game.getBotPlayer(),
			tempPlayer,
			!(game.getPlayerTurn()),
			std::make_shared<Node> (*this) );
		(*raiseNode).setCurrentRaise(raiseAmount);
		raiseChild = raiseNode;
	}
	
	return raiseChild;
}
