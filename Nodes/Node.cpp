#include "Node.h"
#include "../Blinds.h"

#include <memory>
#include <algorithm>	//std::min/max
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
	parent(parent){ }

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
		
		tempPlayer.setChips(tempPlayer.getChips() - (currentRaise - tempPlayer.getPotInvestment()) );
		tempPlayer.setPotInvestment(currentRaise);
		std::cout << "current raise: " << currentRaise << std::endl;
		bool tempAllIn = false;
		if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
				game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
			tempAllIn = true;
		}
		auto callNode = std::make_shared<Node>(game.getState() + !getIsFirst(), //only advances state if not first action taken that stage
			tempPlayer.getPotInvestment() + game.getOppPlayer().getPotInvestment(),
			game.getBoardCards(),
			tempPlayer,
			game.getOppPlayer(),
			!(game.getPlayerTurn()),
			shared_from_this() );
		(*callNode).setIsAllIn(tempAllIn);
        // if first Action, preserve currentRaise, else reset to 0
        (*callNode).setCurrentRaise(getIsFirst() * currentRaise);
		callChild = callNode;
	} else {
		Player tempPlayer = game.getOppPlayer();
		tempPlayer.setChips(tempPlayer.getChips() - (currentRaise - tempPlayer.getPotInvestment()));
		tempPlayer.setPotInvestment(currentRaise);
		bool tempAllIn = false;
		if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
				game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
			tempAllIn = true;
		}
		auto callNode = std::make_shared<Node>( game.getState() + !getIsFirst(),
			game.getBotPlayer().getPotInvestment() + tempPlayer.getPotInvestment(),
			game.getBoardCards(),
			game.getBotPlayer(),
			tempPlayer,
			!(game.getPlayerTurn()),
			shared_from_this() );	
		(*callNode).setIsAllIn(tempAllIn);
        // if first Action, preserve currentRaise, else reset to 0
        (*callNode).setCurrentRaise(getIsFirst() * currentRaise);
		callChild = callNode;
	}
	callChild->setIsFirst(false);
	return callChild;
}

// raiseAmount means amount raising to, NOT raising by
std::shared_ptr<Node> Node::doRaise(double raiseAmount) {
    if (raiseAmount < bigBlind || raiseAmount < 2*currentRaise) {
        raiseAmount = bigBlind > (2*currentRaise) ? bigBlind : (2*currentRaise);
    } 
	// if raise all-in (or more) create AllInNode
	if (game.getBotPlayer().getChips() <= currentRaise ||
				game.getOppPlayer().getChips() <= currentRaise) {
			return doCall();
		}
	if (raiseAmount >= game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() ||
			raiseAmount >= game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() ) {
		std::cout << "Raising All-In" << std::endl;

		// set raiseAmount to lesser of chip amounts
		raiseAmount = std::min(game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment(),
				game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment());
	}
	setIsFirst(false);
	if (game.getPlayerTurn() == 0) {
		Player tempPlayer = game.getBotPlayer();
		tempPlayer.setChips(tempPlayer.getChips() - (raiseAmount - tempPlayer.getPotInvestment()) );
		tempPlayer.setPotInvestment(raiseAmount);
		auto raiseNode = std::make_shared<Node>( game.getState(),
			tempPlayer.getPotInvestment() + game.getOppPlayer().getPotInvestment(),
			game.getBoardCards(),
			tempPlayer,
			game.getOppPlayer(),
			!(game.getPlayerTurn()),
			shared_from_this() );
		(*raiseNode).setCurrentRaise(raiseAmount);
		raiseChild = raiseNode;
	} else {
		Player tempPlayer = game.getOppPlayer();
		tempPlayer.setChips(tempPlayer.getChips() - (raiseAmount - tempPlayer.getPotInvestment()));
		tempPlayer.setPotInvestment(raiseAmount);
		auto raiseNode = std::make_shared<Node>( game.getState(),
			game.getBotPlayer().getPotInvestment() + tempPlayer.getPotInvestment(),
			game.getBoardCards(),
			game.getBotPlayer(),
			tempPlayer,
			!(game.getPlayerTurn()),
			shared_from_this());
		(*raiseNode).setCurrentRaise(raiseAmount);
		raiseChild = raiseNode;
	}
	
	return raiseChild;
}
