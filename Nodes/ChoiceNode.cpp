#include <memory>
#include <iostream>
#include <cassert>

#include "ChoiceNode.h"
#include "OpponentNode.h"
#include "../Stage.h"
#include "../Config.h"
#include "../handEval/helper.h"
#include "../GameUtilities/GameUtilities.h"

ChoiceNode::ChoiceNode() :
    Node() { }

ChoiceNode::ChoiceNode(int state,
        double pot,
        std::vector<int> boardCards,
        Player botPlayer,
        Player oppPlayer,
        int playerTurn,
        Node* const parent) :
    Node(state,
            pot,
            boardCards,
            botPlayer,
            oppPlayer,
            playerTurn,
            parent) { }

ChoiceNode::ChoiceNode(const ChoiceNode& obj) :
    Node(obj) { }

std::unique_ptr<Node>& ChoiceNode::call() {
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
        return callChild;
}
