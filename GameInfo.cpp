#include "Player.h"
#include "GameInfo.h"

#include <vector>

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
