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
    if (thisNode.game.getState() == static_cast<int>(Stage::SHOWDOWN)) 
        runSimulation(thisNode);

    // Expansion
    if (thisNode.callChild == nullptr)
         runSimulation(*(thisNode.call()), deck);
    else if (thisNode.raiseChild == nullptr) {
        // raise function should make this a min raise
         runSimulation(*(thisNode.raise(1)), deck);
    } else if (thisNode.foldChild == nullptr)
         runSimulation(*(thisNode.fold()), deck);

    // Calculate UCT score
    double callScore = naiveUCT(*(thisNode.getCallChild()), exploreConst);
    double raiseScore = naiveUCT(*(thisNode.getRaiseChild()), exploreConst);
    double foldScore = naiveUCT(*(thisNode.getFoldChild()), exploreConst);

    // Pick highest score
    double maxScore = callScore;
    maxScore = maxScore <= raiseScore ? maxScore : raiseScore;
    maxScore = maxScore < foldScore ? maxScore : foldScore;
    if (maxScore == callScore)
        runSelection(*(thisNode.getCallChild()), deck);
    else if (maxScore == raiseScore)
        runSelection(*(thisNode.getRaiseChild()), deck);
    else
        runSelection(*(thisNode.getFoldChild()), deck);
}

void OpponentNode::runSelection(OpponentNode &thisNode, std::vector<int> &deck) {
    if (thisNode.game.getState() == static_cast<int>(Stage::SHOWDOWN)) 
        return thisNode;

    // Expansion
    if (thisNode.callChild == nullptr)
         runSimulation(*(thisNode.call()), deck);
    else if (thisNode.raiseChild == nullptr) {
        // raise function should make this a min raise
         runSimulation(*(thisNode.raise(1)), deck);
    } else if (thisNode.foldChild == nullptr)
         runSimulation(*(thisNode.fold()), deck);

    // Calculate UCT score
    double callScore = naiveUCT(*(thisNode.getCallChild()), exploreConst);
    double raiseScore = naiveUCT(*(thisNode.getRaiseChild()), exploreConst);
    double foldScore = naiveUCT(*(thisNode.getFoldChild()), exploreConst);

    // Pick highest score
    double maxScore = callScore;
    maxScore = maxScore <= raiseScore ? maxScore : raiseScore;
    maxScore = maxScore < foldScore ? maxScore : foldScore;
    if (maxScore == callScore)
        runSelection(*(thisNode.getCallChild()), deck);
    else if (maxScore == raiseScore)
        runSelection(*(thisNode.getRaiseChild()), deck);
    else
        runSelection(*(thisNode.getFoldChild()), deck);
}
 
void OpponentNode::runSimulation(ChoiceNode &thisNode, std::vector<int> deck) {
                    auto copyNode(thisNode);
        while (thisNode.game.getState() != static_cast<int>(Stage::SHOWDOWN)) {
                    auto copyNodeCall = copyNode.call();
                    conditionalDeal(*copyNodeCall, thisNode.game.getState(), copyNodeCall->game.getState(), deck, copyNodeCall->game.getState());
                                }
         backPropagate(copyNode, copyNode.getGame().getBotPlayer().getChips(), copyNode.getGame().getOppPlayer().getChips);
}

void OpponentNode::runSimulation(OpponentNode &thisNode, std::vector<int> deck) {
                    auto copyNode(thisNode);
        while (thisNode.game.getState() != static_cast<int>(Stage::SHOWDOWN)) {
                    auto copyNodeCall = copyNode.call();
                    conditionalDeal(*copyNodeCall, thisNode.game.getState(), copyNodeCall->game.getState(), deck, copyNodeCall->game.getState());
                                }
         backPropagate(copyNode, copyNode.getGame().getBotPlayer().getChips(), copyNode.getGame().getOppPlayer().getChips);
}
			
void ChoiceNode::backPropagate(ChoiceNode& nextNode, double botEV, double oppEV) {
    nextNode.getExpectedValue() = (nextNode.getExpectedValue() * nextNode.getVisitCount() + botEV) / ++(nextNode.getVisitCount());
    if (nextNode.getParent() != nullptr)
        ChoiceNode::backPropagate(*(nextNode.getParent()), nextNode.getExpectedValue(), oppEV);
}

void ChoiceNode::backPropagate(OpponentNode& nextNode, double botEV, double oppEV) {
    nextNode.getExpectedValue() = (nextNode.getExpectedValue() * nextNode.getVisitCount() + oppEV) / ++(nextNode.getVisitCount());
    if (nextNode.getParent() != nullptr)
        backPropagate(*(nextNode.getParent()), botEV, nextNode.getExpectedValue());
}

Action monteCarlo(int maxSeconds, std::vector<int> deck) {
    time_t startTime;
	time(&startTime);
	Node copyNode = *this;
	std::vector<int> copyDeck = deck;
	while (time(0) - startTime < maxSeconds){
	 ChoiceNode::runSelection(copyNode, copyDeck);
	}
	double maxScore = copyNode.getCallChild()->getExpectedValue();
	maxScore = maxScore >= copyNode.getRaiseChild()->getExpectedValue() ? maxScore : copyNode.getRaiseChild()->getExpectedValue();
	maxScore = maxScore >= copyNode.getFoldChild()->getExpectedValue() ? maxScore : copyNode.getFoldChild()->getExpectedValue();
		if (maxScore == copyNode.getCallChild()->getExpectedValue()) {
			return Action::CALL;
		} else if (maxScore == copyNode.getRaiseChild()->getExpectedValue()) {
			// Need to handle how much to raise here
			return Action::RAISE;
		} else {
			return Action::FOLD;
		}

}
