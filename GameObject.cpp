#include "GameObject.h"

GameObject::GameObject(
		int                 state,
		double              pot,
		std::vector<int>    boardCards,
		Player				botPlayer,
		Player				oppPlayer,
		int                 playerTurn) :
	state(state),
	pot(pot),
	boardCards(boardCards),
	botPlayer(botPlayer),
	oppPlayer(oppPlayer),
	playerTurn(playerTurn) { }
