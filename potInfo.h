#ifndef POTINFO_H
#define POTINFO_H

#include "Player.h"

#include <vector>

class PotInfo {
	private:
		const Player botPlayer;
		const Player oppPlayer;
		double potSize;

	public:
		// Constructors
		PotInfo();
		PotInfo(Player bPlayer, Player oPlayer, double psize);

		// Getters
		Player getBotPlayer() const { return botPlayer; }
		Player getOppPlayer() const { return oppPlayer; }
		double getPotSize() const { return potSize; }

		// Pot Modifier
		void addPot(double amount) { potSize += amount; }
};

#endif
