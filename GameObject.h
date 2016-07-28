#ifndef GAME_INFO_H
#define GAME_INFO_H

#include "Player.h"

#include <vector>

class GameObject {
	protected:
		enum states {PREFLOP, FLOP, TURN, RIVER};
		const int state;
		const std::vector<int> boardCards;
		const Player botPlayer;
		const Player oppPlayer;
		const double pot;
		const int playerTurn;

	public:
		// Constructors
		// Doesn't make sense to have default constructor
		GameObject( int                     state,
					double                  pot,
					std::vector<int>        boardCards,
					Player					botPlayer,
					Player					oppPlayer,
					int                     playerTurn);

		// Member-accessibility functions

		// Getters
		int getState() const { return state; }
		std::vector<int> getBoardCards() const { return boardCards; }
		Player getBotPlayer() const { return botPlayer; }
		Player getOppPlayer() const { return oppPlayer; }
		double getPot() const { return pot; }
		int getPlayerTurn() const { return playerTurn; }
};

#endif //GameObject.h
