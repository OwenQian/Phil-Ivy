#ifndef DECISION_H
#define DECISION_H

#include "Action.h"

#include <memory>

class Decision {
	public:
		const Action action;
		const double raiseAmount;

		static Decision makeDecision(std::shared_ptr<ChoiceNode> currentNode);
		static Decision makeDecision(std::shared_ptr<OpponentNode> currentNode);
};

#endif
