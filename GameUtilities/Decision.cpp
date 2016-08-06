#include "Decision.h"
#include "Action.h"

#include <memory>
#include <iostream>

Decision Decision::makeDecision(std::shared_ptr<ChoiceNode> currentNode) {
	std::cout << "Enter Action: Call(0), Raise(1), Fold(2)" << std::endl;
	Decision decision;
	std::cin >> decision.action;
	if (decision.action == RAISE) {
		std::cout << "Enter Raise amount" << std::endl;
		double amount;
		std::cin >> amount;
		decision.raiseAmount = amount;
	}
	return decision;
}

Decision Decision::makeDecision(std::shared_ptr<ChoiceNode> currentNode) {
	std::cout << "Enter Action: Call(0), Raise(1), Fold(2)" << std::endl;
	Decision decision;
	std::cin >> decision.action;
	if (decision.action == RAISE) {
		std::cout << "Enter Raise amount" << std::endl;
		double amount;
		std::cin >> amount;
		decision.raiseAmount = amount;
	}
	return decision;
}
