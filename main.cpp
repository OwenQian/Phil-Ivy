#include "Player.h"
#include "GameObject.h"
#include "Nodes/Node.h"
#include "potInfo.h"
#include "GameUtilities/helper.h"
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

	Player botPlayer(deal(deck, static_cast<int>(Stage::HOLECARDS)), 1000.0, 25.0);
	std::cout << hexToCard(botPlayer.getHoleCards()[0]) << " " << hexToCard(botPlayer.getHoleCards()[1]) << std::endl;
	std::cout << "Deck Size: " << deck.size() << std::endl;
	Player oppPlayer(deal(deck, static_cast<int>(Stage::HOLECARDS) ), 1000.0, 50.0); //update with pot investment
	std::cout << hexToCard(oppPlayer.getHoleCards()[0]) << " " << hexToCard(oppPlayer.getHoleCards()[1]) << std::endl;
	std::cout << "Deck Size: " << deck.size() << std::endl;

	std::vector<int> boardCards;

	auto initialNode = std::make_shared<ChoiceNode>(0, 75.0, boardCards, botPlayer, oppPlayer, 0, std::shared_ptr<ChoiceNode> (NULL));
	std::cout << "Bot Chips: " << (*initialNode).getGame().getBotPlayer().getChips() << std::endl;
	auto f = (*initialNode).raise(1000.0);
	std::cout << "currentRaise amount: " << (*f).getCurrentRaise() << std::endl;
	auto g = (*f).call();
	std::cout << "Bot Chips: " << (*f).getGame().getBotPlayer().getChips() << std::endl;
	std::cout << "Opp Chips: " << (*g).getGame().getBotPlayer().getChips() << std::endl;

	return 0;
}
