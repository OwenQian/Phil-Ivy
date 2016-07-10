#ifndef GAME_INFO_H
#define GAME_INFO_H

#include "Player.h"

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
		//Constructors and Destructors
		GameInfo();
		GameInfo(int state,
				double pot,
				int smallBlind,
				int bigBlind,
				std::vector<int> boardCards,
				std::vector<Player> playerList,
				);
		~GameInfo();

		// Member-accessibility functions
		int getState() const;
		std::vector<int> getBoardCards() const;
		std::vector<Player> getPlayerList() const;
		double getPot() const;
		int getSmallBlind() const;
		int getBigBlind() const;
};

GameInfo::GameInfo() { };

GameInfo::GameInfo(int state,
		double pot,
		int smallBlind,
		int bigBlind,
		std::vector<int> boardCards,
		std::vector<Player> playerList,
		):
	state(state),
	boardCards(boardCards),
	playerList(playerList),
	pot(pot),
	smallBlind(smallBlind),
	bigBlind(bigBlind) { }

int GameInfo::getState() const {
	return state;
}

std::vector<int> GameInfo::getBoardCards() const {
	return boardCards;
}

std::vector<Player> GameInfo::getPlayerList() const {
	return playerList;
}

double GameInfo::getPot() const {
	return pot;
}

int GameInfo::getSmallBlind() {
	return smallBlind;
}

int GameInfo::getBoardCards() {
	return bigBlind;
}

#endif	//GameInfo.h
