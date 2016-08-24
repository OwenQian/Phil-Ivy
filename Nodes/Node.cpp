#include "Node.h"
#include "../Config.h"

#include <memory>
#include <cassert>
#include <algorithm>	//std::min/max
#include <iostream>
#include <cmath>        //for sqrt

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
		bool tempAllIn = false;
		if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
				game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
			tempAllIn = true;
		}
		auto callNode = std::make_shared<Node>(game.getState() + !getIsFirst(), //only advances state if not first action taken that stage
			initialChips * 2 - tempPlayer.getChips() - game.getOppPlayer().getChips(),
			game.getBoardCards(),
			tempPlayer,
			game.getOppPlayer(),
			!(game.getPlayerTurn()),
			shared_from_this() );
		callNode->setIsAllIn(tempAllIn);
        // if first Action, preserve currentRaise and potInv, else reset to 0
        callNode->setCurrentRaise(getIsFirst() * currentRaise);
        callNode->getGame().getBotPlayer().setPotInvestment(getIsFirst() * 
                callNode->getGame().getBotPlayer().getPotInvestment());
        callNode->getGame().getOppPlayer().setPotInvestment(getIsFirst() * 
                callNode->getGame().getOppPlayer().getPotInvestment());
		callChild = callNode;
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
			initialChips * 2 - tempPlayer.getChips() - game.getBotPlayer().getChips(),
			game.getBoardCards(),
			game.getBotPlayer(),
			tempPlayer,
			!(game.getPlayerTurn()),
			shared_from_this() );	
		callNode->setIsAllIn(tempAllIn);
        // if first Action, preserve currentRaise and potInv, else reset to 0
        callNode->setCurrentRaise(getIsFirst() * currentRaise);
        callNode->getGame().getBotPlayer().setPotInvestment(getIsFirst() * 
                callNode->getGame().getBotPlayer().getPotInvestment());
        callNode->getGame().getOppPlayer().setPotInvestment(getIsFirst() * 
                callNode->getGame().getOppPlayer().getPotInvestment());
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
	// if raise all-in (or more) create AllInNode, handled by call
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
			initialChips * 2 - tempPlayer.getChips() - game.getOppPlayer().getChips(),
			game.getBoardCards(),
			tempPlayer,
			game.getOppPlayer(),
			!(game.getPlayerTurn()),
			shared_from_this() );
		raiseNode->setCurrentRaise(raiseAmount);
		raiseChild = raiseNode;
	} else {
		Player tempPlayer = game.getOppPlayer();
		tempPlayer.setChips(tempPlayer.getChips() - (raiseAmount - tempPlayer.getPotInvestment()));
		tempPlayer.setPotInvestment(raiseAmount);
		auto raiseNode = std::make_shared<Node>( game.getState(),
			initialChips * 2 - tempPlayer.getChips() - game.getBotPlayer().getChips(),
			game.getBoardCards(),
			game.getBotPlayer(),
			tempPlayer,
			!(game.getPlayerTurn()),
			shared_from_this());
		raiseNode->setCurrentRaise(raiseAmount);
		raiseChild = raiseNode;
	}
	
	return raiseChild;
}

void Node::naiveUCT(std::vector<double>& selectionScores, double exploreConst) {
    assert(selectionScores.size() == 3);
    std::vector<double> explorationTerm;
    explorationTerm.resize(3);
    int childVisitSum = 0;
    childVisitSum += this->getCallChild()->getVisitCount();
    childVisitSum += this->getFoldChild()->getVisitCount();
    childVisitSum += this->getRaiseChild()->getVisitCount();

    // Order here is important; call, raise, fold (CRF)
    // Set the selectionScore and explorationTerm for call
    selectionScores[0] = this->getCallChild()->getExpectedValue();
    explorationTerm[0] = std::sqrt(
            std::log(double (childVisitSum)) / double (this->getCallChild()->getVisitCount()) );

    // Set the selectionScore and explorationTerm for raise
    selectionScores[1] = this->getRaiseChild()->getExpectedValue();
    explorationTerm[1] = std::sqrt(
            std::log(double (childVisitSum)) / double (this->getRaiseChild()->getVisitCount()) );

    // Set the selectionScore and explorationTerm for fold
    selectionScores[2] = this->getFoldChild()->getExpectedValue();
    explorationTerm[2] = std::sqrt(
            std::log(double (childVisitSum)) / double (this->getFoldChild()->getVisitCount()) );

    for (size_t i = 0; i < selectionScores.size(); ++i) {
        explorationTerm[i] *= exploreConst;
        selectionScores[i] += explorationTerm[i]; 
    }
}
