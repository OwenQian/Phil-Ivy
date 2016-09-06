#include <memory>
#include <iostream>
#include <cassert>

#include "ChoiceNode.h"
#include "OpponentNode.h"
#include "../Config.h"
#include "../GameUtilities/Decision.h"
#include "../GameUtilities/GameUtilities.h"

ChoiceNode::ChoiceNode() :
    Node::Node() { }

ChoiceNode::ChoiceNode(int state,
        double pot,
        std::vector<int> boardCards,
        Player botPlayer,
        Player oppPlayer,
        int playerTurn,
        Node* const parent) :
    Node::Node(state,
            pot,
            boardCards,
            botPlayer,
            oppPlayer,
            playerTurn,
            parent) { }

ChoiceNode::ChoiceNode(const ChoiceNode& obj) :
    Node(obj) { }

void ChoiceNode::fold() {
    foldChild.reset(new ChoiceNode(*this));
    foldChild->setIsFolded(true);
    foldChild->setVisitCount(0);
}

void ChoiceNode::call() {
    Player tempPlayer = game.getBotPlayer();
    tempPlayer.setChips(tempPlayer.getChips() - (currentRaise - tempPlayer.getPotInvestment()) );
    tempPlayer.setPotInvestment(currentRaise);
    bool tempAllIn = false;
    if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
            game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
        tempAllIn = true;
    }
    if (getIsFirst() || smallBlindPosition == 1) {
        callChild.reset( new OpponentNode(game.getState() + !getIsFirst(),
                    initialChips * 2 - tempPlayer.getChips() - game.getOppPlayer().getChips(),
                    game.getBoardCards(),
                    tempPlayer,
                    game.getOppPlayer(),
                    !(game.getPlayerTurn()),
                    this) );
    } else {
        callChild.reset( new ChoiceNode(game.getState() + !getIsFirst(),
                    initialChips * 2 - tempPlayer.getChips() - game.getOppPlayer().getChips(),
                    game.getBoardCards(),
                    tempPlayer,
                    game.getOppPlayer(),
                    !(game.getPlayerTurn()),
                    this) );
    }
    callChild->setIsAllIn(tempAllIn);
    callChild->setCurrentRaise(firstAction * currentRaise);
    callChild->getGame().getBotPlayer().setPotInvestment( firstAction * callChild->getGame().getBotPlayer().getPotInvestment());
    callChild->getGame().getOppPlayer().setPotInvestment( firstAction * callChild->getGame().getOppPlayer().getPotInvestment());
    callChild->setIsFirst(false);
}

void ChoiceNode::raise(double raiseAmount) {
    if (raiseAmount < bigBlind || raiseAmount < 2*currentRaise) {
        raiseAmount = bigBlind > (2*currentRaise) ? bigBlind : (2*currentRaise);
    } 
	// if raise all-in (or more) create AllInNode, handled by call
	if (game.getBotPlayer().getChips() <= currentRaise ||
				game.getOppPlayer().getChips() <= currentRaise) {
			call();
            if (callChild->getGame().getPlayerTurn() == 0) {
                raiseChild.reset(new ChoiceNode(*static_cast<ChoiceNode*>(callChild.get())));
            } else {
                raiseChild.reset(new OpponentNode(*static_cast<OpponentNode*>(callChild.get())));
            }
            std::cout << "raiseChild all in? " << raiseChild->getIsAllIn() << std::endl;
            std::cout << "callChild all in? " <<  callChild->getIsAllIn() << std::endl;
            return;
		}
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
		raiseChild.reset(new OpponentNode( game.getState(),
			initialChips * 2 - tempPlayer.getChips() - game.getOppPlayer().getChips(),
			game.getBoardCards(),
			tempPlayer,
			game.getOppPlayer(),
			!(game.getPlayerTurn()),
			this ));
		raiseChild->setCurrentRaise(raiseAmount);
}

Decision ChoiceNode::makeDecision(std::vector<int> deck) {
    //std::cout << "Enter Action bot: Call(0), Raise(1), Fold(2)" << std::endl;
    Decision decision;
    //int temp;
    //std::cin >> temp;
    //decision.action = static_cast<Action>(temp);
    decision.action = monteCarlo(monteCarloDuration, deck);
    std::cout << "Bot Decision: " << static_cast<int>(decision.action) << std::endl;
    if (decision.action == Action::RAISE) {
        if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
                game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
            decision.raiseAmount = 1;
        } else {
            std::cout << "Enter Raise amount" << std::endl;
            double amount;
            std::cin >> amount;
            decision.raiseAmount = amount;
        }
    }
    return decision;
}
