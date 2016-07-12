#ifndef GAME_INFO_H
#define GAME_INFO_H

#include "Player.h"

#include <vector>

class GameInfo {
	protected:
		const int state;	//TODO make this enum?
		const std::vector<int> boardCards;
		const std::vector<Player> playerList;
		const double pot;
		const int playerTurn;

	public:
		//Constructors and Destructors
		GameInfo();
		GameInfo(int state,
				double pot,
				std::vector<int> boardCards,
				std::vector<Player> playerList,
				int playerTurn
				);

		// Member-accessibility functions
		int getState() const;
		std::vector<int> getBoardCards() const;
		std::vector<Player> getPlayerList() const;
		double getPot() const;
		int getPlayerTurn() const;
	
};

#endif //GameInfo.h
