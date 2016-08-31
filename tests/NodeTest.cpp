#include "../Nodes/ChoiceNode.h"
#include "../Nodes/OpponentNode.h"
#include "../Config.h"
#include "../Player.h"
#include "../GameObject.h"

#include <vector>

int main() {
	int myState = 0;
	double myPot = 10.0;
	std::vector<int> v{1,2};
	Player Max(13, 15, 1000, 0);
	Player Owen(13, 15, 1000, 0);
	int pTurn = 0;
	ChoiceNode n(myState,
			myPot,
			v,
			Max,
			Owen,
			pTurn,
			nullptr);
	n.call();
}
