#include "ChoiceNode.h"
#include "OpponentNode.h"

OpponentNode::OpponentNode(		
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

OpponentNode::OpponentNode(
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

			
void OpponentNode::runSelection(ChoiceNode &thisNode, std::vector<int> &deck) {
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
    // Calculate UCT score
    std::vector<double> selectionScores{0,0,0};
    naiveUCT(selectionScores, exploreConst);

     // Pick highest score
    double maxScore = 0;
    for (size_t i = 0; i < selectionScores.size(); ++i) {
        maxScore = maxScore > selectionScores[i] ? maxScore : selectionScores[i];
    }

    if (maxScore == callScore) {
        if (thisNode.getCallChild()->getGame().getPlayerTurn() == 0)
            runSelection(*std::static_pointer_cast<ChoiceNode>(thisNode.getCallChild()), deck);
        else
            runSelection(*std::static_pointer_cast<OpponentNode>(thisNode.getCallChild()), deck);
    } else if (maxScore == raiseScore) {
        if (thisNode.getRaiseChild()->getGame().getPlayerTurn() == 0)
            runSelection(*std::static_pointer_cast<ChoiceNode>(thisNode.getRaiseChild()), deck);
        else
            runSelection(*std::static_pointer_cast<OpponentNode>(thisNode.getRaiseChild()), deck);
    } else {
        if (thisNode.getFoldChild()->getGame().getPlayerTurn() == 0)
            runSelection(*std::static_pointer_cast<ChoiceNode>(thisNode.getFoldChild()), deck);
        else
            runSelection(*std::static_pointer_cast<OpponentNode>(thisNode.getFoldChild()), deck);
    }
}

void OpponentNode::runSelection(OpponentNode &thisNode, std::vector<int> &deck) {
    if (thisNode.game.getState() == static_cast<int>(Stage::SHOWDOWN)) {
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
    // Calculate UCT score
    std::vector<double> selectionScores{0,0,0};
    naiveUCT(selectionScores, exploreConst);
	
     // Pick highest score
    double maxScore = 0;
    for (size_t i = 0; i < selectionScores.size(); ++i) {
        maxScore = maxScore > selectionScores[i] ? maxScore : selectionScores[i];
    }

    if (maxScore == callScore) {
        if (thisNode.getCallChild()->getGame().getPlayerTurn() == 0)
            runSelection(*std::static_pointer_cast<ChoiceNode>(thisNode.getCallChild()), deck);
        else
            runSelection(*std::static_pointer_cast<OpponentNode>(thisNode.getCallChild()), deck);
    } else if (maxScore == raiseScore) {
        if (thisNode.getRaiseChild()->getGame().getPlayerTurn() == 0)
            runSelection(*std::static_pointer_cast<ChoiceNode>(thisNode.getRaiseChild()), deck);
        else
            runSelection(*std::static_pointer_cast<OpponentNode>(thisNode.getRaiseChild()), deck);
    } else {
        if (thisNode.getFoldChild()->getGame().getPlayerTurn() == 0)
            runSelection(*std::static_pointer_cast<ChoiceNode>(thisNode.getFoldChild()), deck);
        else
            runSelection(*std::static_pointer_cast<OpponentNode>(thisNode.getFoldChild()), deck);
    }
}
 
void OpponentNode::runSimulation(ChoiceNode &thisNode, std::vector<int> deck) {
                    auto copyNode(thisNode);
        while (thisNode.getGame().getState() != static_cast<int>(Stage::SHOWDOWN)) {
                    auto copyNodeCall = copyNode.call();
                    conditionalDeal(*copyNodeCall, thisNode.getGame().getState(), copyNodeCall->game.getState(), deck, copyNodeCall->game.getState());
                                }
         backPropagate(copyNode, copyNode.getGame().getBotPlayer().getChips(), copyNode.getGame().getOppPlayer().getChips());
}

void OpponentNode::runSimulation(OpponentNode &thisNode, std::vector<int> deck) {
                    auto copyNode(thisNode);
        while (thisNode.game.getState() != static_cast<int>(Stage::SHOWDOWN)) {
                    auto copyNodeCall = copyNode.call();
                    conditionalDeal(*copyNodeCall, thisNode.game.getState(), copyNodeCall->getGame().getState(), deck, copyNodeCall->getGame().getState());
                                }
         backPropagate(copyNode, copyNode.getGame().getBotPlayer().getChips(), copyNode.getGame().getOppPlayer().getChips());
}
			
void OpponentNode::backPropagate(ChoiceNode& nextNode, double botEV, double oppEV) {
    nextNode.getExpectedValue() = (nextNode.getExpectedValue() * nextNode.getVisitCount() + botEV) / ++(nextNode.getVisitCount());
    if (nextNode.getParent()) {
        if (nextNode.getParent()->getGame().getPlayerTurn() == 0)
            backPropagate(*std::static_pointer_cast<ChoiceNode>(nextNode.getParent()), nextNode.getExpectedValue(), oppEV);
        else
            backPropagate(*std::static_pointer_cast<OpponentNode>(nextNode.getParent()), nextNode.getExpectedValue(), oppEV);
    }
}

void OpponentNode::backPropagate(OpponentNode& nextNode, double botEV, double oppEV) {
    nextNode.getExpectedValue() = (nextNode.getExpectedValue() * nextNode.getVisitCount() + oppEV) / ++(nextNode.getVisitCount());
    if (nextNode.getParent()) {
        if (nextNode.getParent()->getGame().getPlayerTurn() == 0)
            backPropagate(*std::static_pointer_cast<ChoiceNode>(nextNode.getParent()), botEV, nextNode.getExpectedValue());
        else
            backPropagate(*std::static_pointer_cast<OpponentNode>(nextNode.getParent()), botEV, nextNode.getExpectedValue());
    }
}

//Action OpponentNode::monteCarlo(int maxSeconds, std::vector<int> deck) {
//    time_t startTime;
//	time(&startTime);
//	Node copyNode = *this;
//	std::vector<int> copyDeck = deck;
//	while (time(0) - startTime < maxSeconds){
//	 ChoiceNode::runSelection(copyNode, copyDeck);
//	}
//	double maxScore = copyNode.getCallChild()->getExpectedValue();
//	maxScore = maxScore >= copyNode.getRaiseChild()->getExpectedValue() ? maxScore : copyNode.getRaiseChild()->getExpectedValue();
//	maxScore = maxScore >= copyNode.getFoldChild()->getExpectedValue() ? maxScore : copyNode.getFoldChild()->getExpectedValue();
//		if (maxScore == copyNode.getCallChild()->getExpectedValue()) {
//			return Action::CALL;
//		} else if (maxScore == copyNode.getRaiseChild()->getExpectedValue()) {
//			// Need to handle how much to raise here
//			return Action::RAISE;
//		} else {
//			return Action::FOLD;
//		}
//
//}
