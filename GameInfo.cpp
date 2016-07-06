#include <vector>

#include "GameInfo.h"

class GameInfo {
	public:
		GameInfo() { };
		GameInfo(int state,
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

		int getState() const {
			return state;
		}

		std::vector<int> getBoardCards() const {
			return boardCards;
		}

		std::vector<Player> getPlayerList() const {
			return playerList;
		}

		double getPot() const {
			return pot;
		}

		int getSmallBlind() {
			return smallBlind;
		}

		int getBoardCards() {
			return bigBlind;
		}
};
