#include "ChoiceNode.h"
#include "OpponentNode.h"
#include "../Stage.h"
#include "../Config.h"

#include <memory>
#include <iostream>
#include <chrono>
#include <cassert>

ChoiceNode::ChoiceNode(
							int							state,
							double						pot,
							std::vector<int>			boardCards,
							Player						botPlayer,
							Player						oppPlayer,
							int							playerTurn,
							std::shared_ptr<ChoiceNode>	const parent) :
	Node(	state,
			pot,
			boardCards,
			botPlayer,
			oppPlayer,
			playerTurn,
			parent) { }

ChoiceNode::ChoiceNode(
							int								state,
							double							pot,
							std::vector<int>				boardCards,
							Player							botPlayer,
							Player							oppPlayer,
							int								playerTurn,
							std::shared_ptr<OpponentNode>	const parent) :
	Node(	state,
			pot,
			boardCards,
			botPlayer,
			oppPlayer,
			playerTurn,
			parent) { }
			
void ChoiceNode::runSelection(ChoiceNode &thisNode, std::vector<int> &deck) {
    if (thisNode.game.getState() == static_cast<int>(Stage::SHOWDOWN)) {
        runSimulation (thisNode, deck);
        return;
    }

    // Expansion
    if (!thisNode.callChild) {
         runSimulation(*(thisNode.call()), deck);
         return;
    }
    if (!thisNode.raiseChild) {
        // raise function should make this a min raise
         runSimulation(*(thisNode.raise(1)), deck);
         return;
    } 
    if (!thisNode.getFoldChild()) {
         runSimulation(*(thisNode.fold()), deck);
         return;
    }

    // Calculate UCT score
    std::vector<double> selectionScores{0,0,0};
    thisNode.naiveUCT(selectionScores, exploreConst);
std::cout << "call score: " << selectionScores[0] << std::endl;
	std::cout << "raise score: " << selectionScores[1] << std::endl;
	std::cout << "fold score: " << selectionScores[2] << std::endl;
    // Pick highest score
    double maxScore = 0;
    for (size_t i = 0; i < selectionScores.size(); ++i) {
        maxScore = maxScore > selectionScores[i] ? maxScore : selectionScores[i];
    }
    // Call
    if (maxScore == selectionScores[0]) {
        if (thisNode.getCallChild()->getGame().getPlayerTurn() == 0)
            runSelection(*std::static_pointer_cast<ChoiceNode>(thisNode.getCallChild()), deck);
        else
            runSelection(*std::static_pointer_cast<OpponentNode>(thisNode.getCallChild()), deck);
    // Raise
    } else if (maxScore == selectionScores[1]) {
        if (thisNode.getRaiseChild()->getGame().getPlayerTurn() == 0)
            runSelection(*std::static_pointer_cast<ChoiceNode>(thisNode.getRaiseChild()), deck);
        else
            runSelection(*std::static_pointer_cast<OpponentNode>(thisNode.getRaiseChild()), deck);
    // Fold
    } else {
        if (thisNode.getFoldChild()->getGame().getPlayerTurn() == 0)
            runSelection(*std::static_pointer_cast<ChoiceNode>(thisNode.getFoldChild()), deck);
        else
            runSelection(*std::static_pointer_cast<OpponentNode>(thisNode.getFoldChild()), deck);
    }
}

void ChoiceNode::runSelection(OpponentNode &thisNode, std::vector<int> &deck) {
    if (thisNode.getGame().getState() == static_cast<int>(Stage::SHOWDOWN)) {
        runSimulation(thisNode, deck);
        return;
    }

    // Expansion
    if (!thisNode.getCallChild()) {
         runSimulation(*(thisNode.call()), deck);
         return;
    }
    if (!thisNode.getRaiseChild()) {
        // raise function should make this a min raise
         runSimulation(*(thisNode.raise(1)), deck);
         return;
    } 
    if (!thisNode.getFoldChild()) {
         runSimulation(*(thisNode.fold()), deck);
         return;
    }
    // Calculate UCT score
    std::vector<double> selectionScores{0,0,0};
    thisNode.naiveUCT(selectionScores, exploreConst);

	std::cout << "call score: " << selectionScores[0] << std::endl;
	std::cout << "raise score: " << selectionScores[1] << std::endl;
	std::cout << "fold score: " << selectionScores[2] << std::endl;
    // Pick highest score
    double maxScore = 0;
    for (size_t i = 0; i < selectionScores.size(); ++i) {
        maxScore = maxScore > selectionScores[i] ? maxScore : selectionScores[i];
    }

    // Call
    if (maxScore == selectionScores[0]) {
        if (thisNode.getCallChild()->getGame().getPlayerTurn() == 0)
            runSelection(*std::static_pointer_cast<ChoiceNode>(thisNode.getCallChild()), deck);
        else
            runSelection(*std::static_pointer_cast<OpponentNode>(thisNode.getCallChild()), deck);
    // Raise
    } else if (maxScore == selectionScores[1]) {
        if (thisNode.getRaiseChild()->getGame().getPlayerTurn() == 0)
            runSelection(*std::static_pointer_cast<ChoiceNode>(thisNode.getRaiseChild()), deck);
        else
            runSelection(*std::static_pointer_cast<OpponentNode>(thisNode.getRaiseChild()), deck);
    // Fold
    } else {
        if (thisNode.getFoldChild()->getGame().getPlayerTurn() == 0)
            runSelection(*std::static_pointer_cast<ChoiceNode>(thisNode.getFoldChild()), deck);
        else
            runSelection(*std::static_pointer_cast<OpponentNode>(thisNode.getFoldChild()), deck);
    }
}
 
void ChoiceNode::runSimulation(ChoiceNode &thisNode, std::vector<int> deck) {
	if (!thisNode.getIsFolded()){
    std::shared_ptr<Node> copyNode = std::make_shared<ChoiceNode>(thisNode);
        while (copyNode->getGame().getState() != static_cast<int>(Stage::SHOWDOWN)) {
            if (copyNode->getGame().getPlayerTurn() == 0) {
                auto copyNodeCall = std::static_pointer_cast<ChoiceNode>(copyNode)->call();
                conditionalDeal(*copyNodeCall, copyNode->getGame().getState(), copyNodeCall->getGame().getState(), deck, copyNodeCall->getGame().getState());
                copyNode = copyNodeCall;
            } else {
                auto copyNodeCall = std::static_pointer_cast<OpponentNode>(copyNode)->call();
                conditionalDeal(*copyNodeCall, copyNode->getGame().getState(), copyNodeCall->game.getState(), deck, copyNodeCall->game.getState());
                copyNode = copyNodeCall;
            }
                                                    }
    if (copyNode->getGame().getPlayerTurn() == 0)
        backPropagate(*(std::static_pointer_cast<ChoiceNode>(copyNode)), copyNode->getGame().getBotPlayer().getChips(), copyNode->getGame().getOppPlayer().getChips());
    else
        backPropagate(*(std::static_pointer_cast<OpponentNode>(copyNode)), copyNode->getGame().getBotPlayer().getChips(), copyNode->getGame().getOppPlayer().getChips());
	} else {
		backPropagate(thisNode, thisNode.getGame().getBotPlayer().getChips(), thisNode.getGame().getOppPlayer().getChips());
	}
}

void ChoiceNode::runSimulation(OpponentNode &thisNode, std::vector<int> deck) {
	if (!thisNode.getIsFolded()){
    std::shared_ptr<Node> copyNode = std::make_shared<OpponentNode>(thisNode);
        while (copyNode->getGame().getState() != static_cast<int>(Stage::SHOWDOWN)) {
            if (copyNode->getGame().getPlayerTurn() == 0) {
                auto copyNodeCall = std::static_pointer_cast<ChoiceNode>(copyNode)->call();
                conditionalDeal(*copyNodeCall, copyNode->getGame().getState(), copyNodeCall->getGame().getState(), deck, copyNodeCall->getGame().getState());
                copyNode = copyNodeCall;
            } else {
                auto copyNodeCall = std::static_pointer_cast<OpponentNode>(copyNode)->call();
                conditionalDeal(*copyNodeCall, copyNode->getGame().getState(), copyNodeCall->game.getState(), deck, copyNodeCall->game.getState());
                copyNode = copyNodeCall;
            }
                                                    }
    if (copyNode->getGame().getPlayerTurn() == 0)
        backPropagate(*(std::static_pointer_cast<ChoiceNode>(copyNode)), copyNode->getGame().getBotPlayer().getChips(), copyNode->getGame().getOppPlayer().getChips());
    else
        backPropagate(*(std::static_pointer_cast<OpponentNode>(copyNode)), copyNode->getGame().getBotPlayer().getChips(), copyNode->getGame().getOppPlayer().getChips());
	} else {
		backPropagate(thisNode, thisNode.getGame().getBotPlayer().getChips(), thisNode.getGame().getOppPlayer().getChips());
	}
}
    
void ChoiceNode::backPropagate(ChoiceNode& nextNode, double botEV, double oppEV) {
    nextNode.getExpectedValue() = (nextNode.getExpectedValue() * nextNode.getVisitCount() + botEV) / ++(nextNode.getVisitCount());
    if (nextNode.getParent()) {
        if (nextNode.getParent()->getGame().getPlayerTurn() == 0)
            backPropagate(*std::static_pointer_cast<ChoiceNode>(nextNode.getParent()), nextNode.getExpectedValue(), oppEV);
        else
            backPropagate(*std::static_pointer_cast<OpponentNode>(nextNode.getParent()), nextNode.getExpectedValue(), oppEV);
    }
}

void ChoiceNode::backPropagate(OpponentNode& nextNode, double botEV, double oppEV) {
    nextNode.getExpectedValue() = (nextNode.getExpectedValue() * nextNode.getVisitCount() + oppEV) / ++(nextNode.getVisitCount());
    if (nextNode.getParent()) {
        if (nextNode.getParent()->getGame().getPlayerTurn() == 0)
            backPropagate(*std::static_pointer_cast<ChoiceNode>(nextNode.getParent()), botEV, nextNode.getExpectedValue());
        else
            backPropagate(*std::static_pointer_cast<OpponentNode>(nextNode.getParent()), botEV, nextNode.getExpectedValue());
    }
}

Action ChoiceNode::monteCarlo(int maxSeconds, std::vector<int> deck) {
    time_t startTime;
	time(&startTime);
    std::shared_ptr<ChoiceNode> copyNode = std::make_shared<ChoiceNode>(*this);
	std::vector<int> copyDeck = deck;
	while (time(0) - startTime < maxSeconds){
        runSelection(*copyNode, deck);
	}
	double maxScore = copyNode->getCallChild()->getExpectedValue();
	maxScore = maxScore >= copyNode->getRaiseChild()->getExpectedValue() ? maxScore : copyNode->getRaiseChild()->getExpectedValue();
	maxScore = maxScore >= copyNode->getFoldChild()->getExpectedValue() ? maxScore : copyNode->getFoldChild()->getExpectedValue();
		if (maxScore == copyNode->getCallChild()->getExpectedValue()) {
			return Action::CALL;
		} else if (maxScore == copyNode->getRaiseChild()->getExpectedValue()) {
			// Need to handle how much to raise here
			return Action::RAISE;
		} else {
			return Action::FOLD;
		}

}
