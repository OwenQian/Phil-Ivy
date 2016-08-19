#include <iostream>
#include <memory>

#include "ChoiceNode.h"
#include "OpponentNode.h"
#include "../Blinds.h"

class ChoiceNode;
class OpponentNode;
class Node;

OpponentNode::OpponentNode(		
		int							state,
		double						pot,
		std::vector<int>			boardCards,
		Player						botPlayer,
		Player						oppPlayer,
		int							playerTurn,
		std::shared_ptr<Node>	const parent) :
	Node(	state,
			pot,
			boardCards,
			botPlayer,
			oppPlayer,
			playerTurn,
			parent) { }

Node& OpponentNode::foldImplementation() {
	//create child foldNode
	auto foldNode = std::make_shared<ChoiceNode>(game.getState(),
			game.getPot(),
			game.getBoardCards(),
			game.getBotPlayer(),
			game.getOppPlayer(),
			game.getPlayerTurn(),
			this->parent);
	foldNode->setIsFolded(true);
	return *foldNode;
}

Node& OpponentNode::callImplementation() {
	//creates a temporary playerlist and updates the player's potinvestment and chip count
	Player tempPlayer = game.getOppPlayer();
	tempPlayer.setChips(tempPlayer.getChips() - (currentRaise - tempPlayer.getPotInvestment()) );
	tempPlayer.setPotInvestment(currentRaise);
	std::cout << "current raise: " << currentRaise << std::endl;
	bool tempAllIn = false;
	// if the call puts us all in, put in the rest of our chips and set to all in
	if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
			game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
		tempAllIn = true;
	}
	//only advances state if not first action taken that stage
	auto callNode = std::make_shared<ChoiceNode>(game.getState() + !getIsFirst(), 
			tempPlayer.getPotInvestment() + game.getBotPlayer().getPotInvestment(),
			game.getBoardCards(),
			game.getBotPlayer(),
			tempPlayer,
			!(game.getPlayerTurn()),
			shared_from_this() );
	callNode->setIsAllIn(tempAllIn);
	// if first Action, preserve currentRaise, else reset to 0
	callNode->setCurrentRaise(getIsFirst() * currentRaise);
	//callChild = callNode;
	callChild->setIsFirst(false);
	return *callNode;
}

Node& OpponentNode::raiseImplementation(double raiseAmount) {
	// if inadequate raise amount, set to min-raise(2*currentRaise or by bigBlind)
	if (raiseAmount < bigBlind || raiseAmount < 2*currentRaise) {
		raiseAmount = bigBlind > (2*currentRaise) ? bigBlind : (2*currentRaise);
	} 
	// if we don't have enough chips to raise, default to calling
	if (game.getBotPlayer().getChips() <= currentRaise ||
			game.getOppPlayer().getChips() <= currentRaise) {
		return call();
	}
	// if raise puts one of the players all-in, adjust raise amount so we don't overraise
	if (raiseAmount >= game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() ||
			raiseAmount >= game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() ) {
		std::cout << "Raising All-In" << std::endl;
		// set raiseAmount to lesser of chip amounts
		raiseAmount = std::min(game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment(),
				game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment());
	}
	setIsFirst(false);
	Player tempPlayer = game.getOppPlayer();
	tempPlayer.setChips(tempPlayer.getChips() - (raiseAmount - tempPlayer.getPotInvestment()) );
	tempPlayer.setPotInvestment(raiseAmount);
	auto raiseNode = std::make_shared<ChoiceNode>(
			game.getState(),
			tempPlayer.getPotInvestment() + game.getBotPlayer().getPotInvestment(),
			game.getBoardCards(),
			game.getBotPlayer(),
			tempPlayer,
			!(game.getPlayerTurn()),
			shared_from_this() );
	raiseNode->setCurrentRaise(raiseAmount);
	return *raiseNode;
}

ChoiceNode& OpponentNode::call() {
	return dynamic_cast<ChoiceNode&>(callImplementation());
}

ChoiceNode& OpponentNode::raise(double raiseAmount) {
	return dynamic_cast<ChoiceNode&>(raiseImplementation(raiseAmount));
}

ChoiceNode& OpponentNode::fold() {
	return dynamic_cast<ChoiceNode&>(foldImplementation());
}
