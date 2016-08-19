#include <memory>
#include <iostream>

#include "Decision.h"
#include "Action.h"
#include "../Nodes/ChoiceNode.h"
#include "../Nodes/OpponentNode.h"

Decision Decision::makeDecision(ChoiceNode currentNode) {
	std::cout << "Enter Action bot: Call(0), Raise(1), Fold(2)" << std::endl;
	Decision decision;
	int temp;
	std::cin >> temp;
	decision.action = static_cast<Action> (temp);
	if (decision.action == Action::RAISE) {
		std::cout << "Enter Raise amount" << std::endl;
		double amount;
		std::cin >> amount;
		decision.raiseAmount = amount;
	}
	return decision;
}

Decision Decision::makeDecision(OpponentNode currentNode) {
	std::cout << "Enter Action opp: Call(0), Raise(1), Fold(2)" << std::endl;
	Decision decision;
	int temp;
	std::cin >> temp;
	decision.action = static_cast<Action> (temp);
	if (decision.action == Action::RAISE) {
		std::cout << "Enter Raise amount" << std::endl;
		double amount;
		std::cin >> amount;
		decision.raiseAmount = amount;
	}
	return decision;
}
