#ifndef DECISION_H
#define DECISION_H

#include "Action.h"

#include <memory>

class ChoiceNode;
class OpponentNode;

class Decision {
	public:
		Action action;
		double raiseAmount;
		
		static Decision makeDecision(ChoiceNode currentNode);
		static Decision makeDecision(OpponentNode currentNode);
};

#endif
