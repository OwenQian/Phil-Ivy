#include "Player.h"
#include "GameInfo.h"
#include <vector>

GameInfo::GameInfo() { };

GameInfo::GameInfo(int state,
		double pot,
		std::vector<int> boardCards,
		std::vector<Player> playerList,
		int playerTurn):
	state(state),
	boardCards(boardCards),
	playerList(playerList),
	pot(pot) { }

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

int GameInfo::getBoardCards() {
	return bigBlind;
}

#endif	//GameInfo.h
