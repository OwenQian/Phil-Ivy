#ifndef POTINFO_H
#define POTINFO_H

#include "Player.h"

#include <vector>

class PotInfo {
	private:
		const std::vector<Player> playerList;
		double potSize;

	public:
		// Constructors
		PotInfo();
		PotInfo(std::vector<Player> plist, double psize);

		// Getters
		std::vector<Player> getPlayerList() const { return playerList; }
		double getPotSize() const { return potSize; }

		// Pot Modifier
		void addPot(double amount) { potSize += amount; }
};

#endif
