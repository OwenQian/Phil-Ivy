#ifndef PLAYER_H
#define PLAYER_H

#include <vector>

class Player {
	private:
		std::vector<int> holeCards;
		double chips;
		double potInvestment;

	public:
		// member accessibility fcns
		std::vector<int> getHoleCards();
		double getChips();
		double getPotInvestment();
};

std::vector<int> Player::getHoleCards() {
	return holeCards;
}

double Player::getChips() {
	return chips;
}

double Player::getPotInvestment() {
	return potInvestment;
}

#endif
