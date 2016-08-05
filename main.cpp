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
	
	Player botPlayer(deal(deck, HOLECARDS), 1000.0, 25.0);
	std::cout << hexToCard(botPlayer.getHoleCards()[0]) << " " << hexToCard(botPlayer.getHoleCards()[1]) << std::endl;
	std::cout << "Deck Size: " << deck.size() << std::endl;
	Player oppPlayer(deal(deck, HOLECARDS), 1000.0, 50.0); //update with pot investment
	std::cout << hexToCard(oppPlayer.getHoleCards()[0]) << " " << hexToCard(oppPlayer.getHoleCards()[1]) << std::endl;
	std::cout << "Deck Size: " << deck.size() << std::endl;
	
	std::vector<int> boardCards;
	
	auto initialNode = std::make_shared<ChoiceNode>(0, 75.0, boardCards, botPlayer, oppPlayer, 0, std::shared_ptr<ChoiceNode> (NULL));
	auto f = (*initialNode).doRaise(5000.0);
	std::cout << "currentRaise amount: " << (*f).getCurrentRaise() << std::endl;
	auto g = (*f).doCall();
	std::cout << "currentRaise amount: " << (*g).getCurrentRaise() << std::endl;
	std::cout << "Player Turn: " << (*f).getGame().getPlayerTurn() << std::endl;
	std::cout << "Player Turn: " << (*g).getGame().getPlayerTurn() << std::endl;
	std::cout << (*f).getIsAllIn() << std::endl;
	std::cout << (*g).getIsAllIn() << std::endl;
	
	return 0;
}
