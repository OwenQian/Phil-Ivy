#include "GameObject.h"

GameObject::GameObject( int                 state,
						double              pot,
						std::vector<int>    boardCards,
						std::vector<Player>	playerList,
						int                 playerTurn) :
	state(state),
	pot(pot),
	boardCards(boardCards),
	playerList(playerList),
	playerTurn(playerTurn) { }
