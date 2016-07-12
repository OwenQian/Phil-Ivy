#ifndef POTINFO_H
#define POTINFO_H

#include <vector>

class PotInfo {
	private:
		std::vector<Player> playerList;
		double potSize;
	
	public:
		PotInfo();
		PotInfo(std::vector<Player> plist, double psize);
		std::vector<Player> getPlayers();
		double getPotSize();
		void addPot(double amount);
	
	PotInfo(){
		potSize = 0;
	}

	PotInfo(std::vector<Player> plist, double psize) {
		playerList = plist;
		potSize = psize;
	}
		
	std::vector<Player> getPlayers() {
		return playerList;
	}	

	double getPotSize() {
		return potSize;
	}

	void addPot(double amount) {
		potSize += amount;
	}
};

#endif


