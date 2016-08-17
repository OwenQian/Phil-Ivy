#ifndef CHOICE_NODE_H
#define CHOICE_NODE_H

#include "Node.h"
#include "OpponentNode.h"
#include "../GameObject.h"
#include "../GameUtilities/GameUtilities.h"
#include "../Stage.h"

#include <memory>		//smart pointer

class OpponentNode;

class ChoiceNode: public Node {	//inheriting from Node class
	private:
        // Monte Carlo functions
        Action monteCarlo(int maxSeconds);
        template <typename NodeType> Node& runSelection(NodeType& thisNode);
        template <typename NodeType> Node& runSimulation(NodeType& thisNode, std::vector<int> deck);
        void backPropagate(ChoiceNode& nextNode, double botEV, double oppEV);
        void backPropagate(OpponentNode& nextNode, double botEV, double oppEV);


	public:
		// Constructor
		ChoiceNode(
				int								state,
				double							pot,
				std::vector<int>				boardCards,
				Player							botPlayer,
				Player							oppPlayer,
				int								playerTurn,
				std::shared_ptr<OpponentNode>	const parent);

		ChoiceNode(
				int								state,
				double							pot,
				std::vector<int>				boardCards,
				Player							botPlayer,
				Player							oppPlayer,
				int								playerTurn,
				std::shared_ptr<ChoiceNode>		const parent);

		// Wrapper for action functions to return correct type
		std::shared_ptr<OpponentNode> fold() {
			return std::static_pointer_cast<OpponentNode> (doFold());
		}
		std::shared_ptr<OpponentNode> call() {
			return std::static_pointer_cast<OpponentNode> (doCall());
		}
		std::shared_ptr<OpponentNode> raise(double raiseAmount) {
			return std::static_pointer_cast<OpponentNode> (doRaise(raiseAmount));
		}
};

template <typename NodeType> Node& ChoiceNode::runSelection(NodeType& thisNode) {
    if (thisNode.game.getState() == static_cast<int>(Stage::SHOWDOWN)) 
        return thisNode;

    // Expansion
    if (thisNode.callChild == std::shared_ptr<Node>(NULL))
        return *(thisNode.call());
    else if (thisNode.raiseChild == std::shared_ptr<Node>(NULL)) {
        // raise function should make this a min raise
        return *(thisNode.raise(1));
    } else if (thisNode.foldChild == std::shared_ptr<Node>(NULL))
        return *(thisNode.fold());

    // Calculate UCT score
    double callScore = naiveUCT(*(thisNode.getCallChild()), exploreConst);
    double raiseScore = naiveUCT(*(thisNode.getRaiseChild()), exploreConst);
    double foldScore = naiveUCT(*(thisNode.getFoldChild()), exploreConst);

    // Pick highest score
    double maxScore = callScore;
    maxScore = maxScore <= raiseScore ? maxScore : raiseScore;
    maxScore = maxScore < foldScore ? maxScore : foldScore;
    if (maxScore == callScore)
        runSelection(*(thisNode->getCallChild()));
    else if (maxScore == raiseScore)
        runSelection(*(thisNode->getRaiseChild()));
    else
        runSelection(*(thisNode->getFoldChild()));
}
 
template <typename NodeType> Node& ChoiceNode::runSimulation(NodeType& thisNode, std::vector<int> deck) {
                    auto copyNode(thisNode);
        while (thisNode.game.getState() != static_cast<int>(Stage::SHOWDOWN)) {
                    auto copyNodeCall = copyNode.call();
                    conditionalDeal(*copyNodeCall, thisNode.game.getState(), copyNodeCall->game.getState(), deck, copyNodeCall->game.getState());
                                }
        return copyNode;
}
#endif //Node.h
