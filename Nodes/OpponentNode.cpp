#include <memory>
#include <iostream>
#include <cassert>

#include "ChoiceNode.h"
#include "OpponentNode.h"
#include "../Config.h"
#include "../Action.h"
#include "../GameUtilities/Decision.h"
#include "../GameUtilities/GameUtilities.h"

OpponentNode::OpponentNode() :
    Node::Node() { }

OpponentNode::OpponentNode(int state,
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

OpponentNode::OpponentNode(const OpponentNode& obj) :
    Node(obj) { }

std::unique_ptr<Node>& OpponentNode::fold() {
    foldChild.reset(new OpponentNode(*this));
    foldChild->setIsFolded(true);
    foldChild->setVisitCount(0);
    return foldChild;
}

std::unique_ptr<Node>& OpponentNode::call() {
    Player tempPlayer = game.getOppPlayer();
    tempPlayer.setChips(tempPlayer.getChips() - (currentRaise - tempPlayer.getPotInvestment()) );
    tempPlayer.setPotInvestment(currentRaise);
    bool tempAllIn = false;
		if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
				game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
			tempAllIn = true;
		}
        if (getIsFirst() || smallBlindPosition == 0) {
        callChild.reset( new ChoiceNode(game.getState() + !getIsFirst(),
                    initialChips * 2 - tempPlayer.getChips() - game.getBotPlayer().getChips(),
                    game.getBoardCards(),
                    game.getBotPlayer(),
                    tempPlayer,
                    !(game.getPlayerTurn()),
                    this) );
        } else {
             callChild.reset( new OpponentNode(game.getState() + !getIsFirst(),
                    initialChips * 2 - tempPlayer.getChips() - game.getBotPlayer().getChips(),
                    game.getBoardCards(),
                    game.getBotPlayer(),
                    tempPlayer,
                    !(game.getPlayerTurn()),
                    this) );
        }
        callChild->setIsAllIn(tempAllIn);
        callChild->setCurrentRaise(firstAction * currentRaise);
        callChild->getGame().getBotPlayer().setPotInvestment( firstAction * callChild->getGame().getBotPlayer().getPotInvestment());
        callChild->getGame().getOppPlayer().setPotInvestment( firstAction * callChild->getGame().getOppPlayer().getPotInvestment());
        return callChild;
}

std::unique_ptr<Node>& OpponentNode::raise(double raiseAmount) {
    if (raiseAmount < bigBlind || raiseAmount < 2*currentRaise) {
        raiseAmount = bigBlind > (2*currentRaise) ? bigBlind : (2*currentRaise);
    } 
	// if raise all-in (or more) create AllInNode, handled by call
	if (game.getBotPlayer().getChips() <= currentRaise ||
				game.getOppPlayer().getChips() <= currentRaise) {
			return call();
		}
	if (raiseAmount >= game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() ||
			raiseAmount >= game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() ) {
		std::cout << "Raising All-In" << std::endl;

		// set raiseAmount to lesser of chip amounts
		raiseAmount = std::min(game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment(),
				game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment());
	}
		Player tempPlayer = game.getOppPlayer();
		tempPlayer.setChips(tempPlayer.getChips() - (raiseAmount - tempPlayer.getPotInvestment()) );
		tempPlayer.setPotInvestment(raiseAmount);
		raiseChild.reset(new ChoiceNode( game.getState(),
			initialChips * 2 - tempPlayer.getChips() - game.getBotPlayer().getChips(),
			game.getBoardCards(),
			game.getBotPlayer(),
			tempPlayer,
			!(game.getPlayerTurn()),
			this ));
		raiseChild->setCurrentRaise(raiseAmount);
	
	return raiseChild;
}

Decision OpponentNode::makeDecision() {
    std::cout << "Enter Action opp: Call(0), Raise(1), Fold(2)" << std::endl;
    Decision decision;
    int temp;
    std::cin >> temp;
    std::vector<int> deck;
    init_deck(deck);
    decision.action = static_cast<Action>(temp);
    std::cout << "Opp Decision: " << static_cast<int>(decision.action) << std::endl;
    if (decision.action == Action::RAISE) {
        std::cout << "Enter Raise amount" << std::endl;
        double amount;
        std::cin >> amount;
        decision.raiseAmount = amount;
    }
    return decision;
}
