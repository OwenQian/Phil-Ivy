#include "Player.h"
#include "GameObject.h"
#include "Nodes/Node.h"
#include "potInfo.h"
#include "GameUtilities/helper.h"
#include "GameUtilities/GameUtilities.h"
#include "Nodes/ChoiceNode.h"
#include "Nodes/OpponentNode.h"

#include <vector>
#include <memory>
#include <iostream>

int main() {
	enum states {HOLECARDS, PREFLOP, FLOP, TURN, RIVER};
	std::vector<int> deck;
	init_deck(deck);
//	double bigBlind = 50.0;
//	double smallBlind = 25.0;
//	int smallBlindPosition = 0;
	
	Player botPlayer(deal(deck, HOLECARDS), 1000.0, 25.0);
	std::cout << hexToCard(botPlayer.getHoleCards()[0]) << " " << hexToCard(botPlayer.getHoleCards()[1]) << std::endl;
	std::cout << "Deck Size: " << deck.size() << std::endl;
	Player oppPlayer(deal(deck, HOLECARDS), 1000.0, 50.0); //update with pot investment
	std::cout << hexToCard(oppPlayer.getHoleCards()[0]) << " " << hexToCard(oppPlayer.getHoleCards()[1]) << std::endl;
	std::cout << "Deck Size: " << deck.size() << std::endl;
	
	std::vector<int> boardCards;
	
	auto initialNode = std::make_shared<ChoiceNode>(0, 75.0, boardCards, botPlayer, oppPlayer, 0, std::shared_ptr<ChoiceNode> (NULL));
	auto f = (*initialNode).doCall(25.0);
	
	std::cout << "botPlayer chip count, expected 975 :" << (*f).getGame().getBotPlayer().getChips() <<std::endl;
	std::cout << "pot chip count, expected 100 :" << (*f).getGame().getPot() <<std::endl;
	
	auto d = (*f).raise(500.0);
	std::cout << "botPlayer chip count, expected 500 :" << (*d).getGame().getOppPlayer().getChips() <<std::endl;
	std::cout << "pot chip count, expected 600 :" << (*d).getGame().getPot() <<std::endl;
	return 0;
}
