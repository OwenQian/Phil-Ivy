#ifndef CHOICE_NODE_H
#define CHOICE_NODE_H

#include "Node.h"
#include "../GameObject.h"
#include "../GameUtilities/GameUtilities.h"
#include "../Stage.h"
#include "../GameUtilities/Action.h"

#include <memory>		//smart pointer

class OpponentNode;
class ChoiceNode: public Node {	//inheriting from Node class
    public:
        Action monteCarlo(int maxSeconds, std::vector<int>);
	private:
        // Monte Carlo functions
        void runSelection(ChoiceNode& thisNode, std::vector<int> &deck);
		void runSelection(OpponentNode& thisNode, std::vector<int> &deck);
        void runSimulation(ChoiceNode& thisNode, std::vector<int> deck);
		void runSimulation(OpponentNode& thisNode, std::vector<int> deck);
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

#endif //Node.h
