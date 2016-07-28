#ifndef GAME_INFO_H
#define GAME_INFO_H

#include "Player.h"

#include <vector>

class GameObject {
	protected:
		enum states {PREFLOP, FLOP, TURN, RIVER};
		const int state;
		const std::vector<int> boardCards;
		const std::vector<Player> playerList;
		const double pot;
		const int playerTurn;

	public:
		// Constructors
		// Doesn't make sense to have default constructor
		GameObject( int                     state,
					double                  pot,
					std::vector<int>        boardCards,
					std::vector<Player>     playerList,
					int                     playerTurn);

		// Member-accessibility functions

		// Getters
		int getState() const { return state; }
		std::vector<int> getBoardCards() const { return boardCards; }
		std::vector<Player> getPlayerList() const { return playerList; }
		double getPot() const { return pot; }
		int getPlayerTurn() const { return playerTurn; }
};

#endif //GameObject.h
