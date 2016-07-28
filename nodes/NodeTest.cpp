#include "Node.h"

#include <vector>

int main() {
	static const double smallBlind = 5.0;
	static const double bigBlind = 10.0;
	int myState = 0;
	double myPot = 10.0;
	std::vector<int> v{1,2};
	Player Max(13, 15, 1000, 0);
	Player Owen(13, 15, 1000, 0);
	int pTurn = 2;
	Node n(myState,
			myPot,
			v,
			Max,
			Owen,
			pTurn);
}
