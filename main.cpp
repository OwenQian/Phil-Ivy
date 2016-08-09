#include "Player.h"
#include "GameObject.h"
#include "Nodes/Node.h"
#include "potInfo.h"
#include "handEval/helper.h"
#include "GameUtilities/GameUtilities.h"
#include "Nodes/ChoiceNode.h"
#include "Nodes/OpponentNode.h"
#include "Stage.h"

#include <vector>
#include <memory>
#include <iostream>

int main() {
	std::vector<int> deck;
	init_deck(deck);

	Player botPlayer(deal(deck, static_cast<int>(Stage::HOLECARDS)), 1000.0, 0.0);
	Player oppPlayer(deal(deck, static_cast<int>(Stage::HOLECARDS)), 1000.0, 0.0);

	std::cout << "botPlayer cards: " << hexToCard(botPlayer.getHoleCards()[0]) << " " << hexToCard(botPlayer.getHoleCards()[1]) << std::endl;
	std::cout << "oppPlayer cards: " << hexToCard(oppPlayer.getHoleCards()[0]) << " " << hexToCard(oppPlayer.getHoleCards()[1]) << std::endl;
	
	std::cout << "bot chips: " << botPlayer.getChips() << std::endl;
	std::cout << "opp chips: " << oppPlayer.getChips() << std::endl;
	playRound(botPlayer, oppPlayer);

	return 0;
}

