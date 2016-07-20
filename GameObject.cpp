#include "Player.h"
#include "GameObject.h"
#include <vector>

GameObject::GameObject() { };

GameObject::GameObject(int state,
		double pot,
		std::vector<int> boardCards,
		std::vector<Player> playerList,
		int playerTurn):
	state(state),
	boardCards(boardCards),
	playerList(playerList),
	pot(pot) { }

	int GameObject::getState() const {
		return state;
	}

std::vector<int> GameObject::getBoardCards() const {
	return boardCards;
}

std::vector<Player> GameObject::getPlayerList() const {
	return playerList;
}

double GameObject::getPot() const {
	return pot;
}

int GameObject::getBoardCards() {
	return bigBlind;
}

int GameObject::getPlayerTurn() const {
	return playerTurn;
}

#endif	//GameObject.h
