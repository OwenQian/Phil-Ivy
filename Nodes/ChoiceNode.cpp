#include <memory>
#include <iostream>

#include "ChoiceNode.h"
#include "../Blinds.h"
#include "OpponentNode.h"

class ChoiceNode;
class OpponentNode;
class Node;

	ChoiceNode::ChoiceNode(
			int								state,
			double							pot,
			std::vector<int>				boardCards,
			Player							botPlayer,
			Player							oppPlayer,
			int								playerTurn,
			std::shared_ptr<Node>	const parent) :
		Node(	state,
				pot,
				boardCards,
				botPlayer,
				oppPlayer,
				playerTurn,
				parent) { }

Node& ChoiceNode::foldImplementation() {
    //create child foldNode
    auto foldNode = std::make_shared<OpponentNode>(game.getState(),
                                                    game.getPot(),
                                                    game.getBoardCards(),
                                                    game.getBotPlayer(),
                                                    game.getOppPlayer(),
                                                    game.getPlayerTurn(),
                                                    this->parent);
    foldNode->setIsFolded(true);
    return *foldNode;
}

Node& ChoiceNode::callImplementation() {
	//creates a temporary playerlist and updates the player's potinvestment and chip count
	Player tempPlayer = game.getBotPlayer();
	tempPlayer.setChips(tempPlayer.getChips() - (currentRaise - tempPlayer.getPotInvestment()) );
	tempPlayer.setPotInvestment(currentRaise);
	std::cout << "current raise: " << currentRaise << std::endl;
	bool tempAllIn = false;
	if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
			game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
		tempAllIn = true;
	}
	auto callNode = std::make_shared<OpponentNode>(game.getState() + !getIsFirst(), //only advances state if not first action taken that stage
			tempPlayer.getPotInvestment() + game.getOppPlayer().getPotInvestment(),
			game.getBoardCards(),
			tempPlayer,
			game.getOppPlayer(),
			!(game.getPlayerTurn()),
			shared_from_this() );
	callNode->setIsAllIn(tempAllIn);
	// if first Action, preserve currentRaise, else reset to 0
	callNode->setCurrentRaise(getIsFirst() * currentRaise);
	//callChild = callNode;
	callChild->setIsFirst(false);
	return *callNode;
}

// raiseAmount means amount raising to, NOT raising by
Node& ChoiceNode::raiseImplementation(double raiseAmount) {
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
	Player tempPlayer = game.getBotPlayer();
	tempPlayer.setChips(tempPlayer.getChips() - (raiseAmount - tempPlayer.getPotInvestment()) );
	tempPlayer.setPotInvestment(raiseAmount);
	auto raiseNode = std::make_shared<OpponentNode>( game.getState(),
			tempPlayer.getPotInvestment() + game.getOppPlayer().getPotInvestment(),
			game.getBoardCards(),
			tempPlayer,
			game.getOppPlayer(),
			!(game.getPlayerTurn()),
			shared_from_this() );
	raiseNode->setCurrentRaise(raiseAmount);
	//raiseChild = raiseNode;
	return *raiseNode;
}

OpponentNode& ChoiceNode::call() {
	return dynamic_cast<OpponentNode&>(callImplementation());
}

OpponentNode& ChoiceNode::raise(double raiseAmount) {
	return dynamic_cast<OpponentNode&>(raiseImplementation(raiseAmount));
}

OpponentNode& ChoiceNode::fold() {
	return dynamic_cast<OpponentNode&>(foldImplementation());
}
