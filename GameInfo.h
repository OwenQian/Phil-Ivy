#ifndef GAME_INFO_H
#define GAME_INFO_H

#include <vector>

class GameInfo {
	protected:
		int state;	//TODO make this enum?
		std::vector<int> boardCards;
		std::vector<Player> playerList;
		double pot;
		int smallBlind;
		int bigBlind;

	public:
		GameInfo();
		GameInfo(int state,
				double pot,
				int smallBlind,
				int bigBlind,
				std::vector<int> boardCards,
				std::vector<Player> playerList,
				);
		~GameInfo();
		int getState() const;
		std::vector<int> getBoardCards() const;
		std::vector<Player> getPlayerList() const;
		double getPot() const;
		int getSmallBlind();
		int getBigBlind();
}

#endif	//GameInfo.h
