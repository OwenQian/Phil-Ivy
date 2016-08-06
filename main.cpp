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

	Player botPlayer(deal(deck, static_cast<int>(Stage::PREFLOP)), 1000.0, 0.0);
	Player oppPlayer(deal(deck, static_cast<int>(Stage::PREFLOP)), 1000.0, 0.0);

	std::vector<int> boardCards{cardToHex("Ac"), cardToHex("Kd"), cardToHex("3h"), cardToHex("2s"), cardToHex("7h")};
	for (auto i = boardCards.begin(); i != boardCards.end(); ++i)
		std::cout << hexToCard(*i) << " " ;
	std::cout << "\n";
	std::cout << "botCards: " << hexToCard(botPlayer.getHoleCards()[0]) << " " << hexToCard(botPlayer.getHoleCards()[1]) << std::endl;
	std::cout << "oppCards: " << hexToCard(oppPlayer.getHoleCards()[0]) << " " << hexToCard(oppPlayer.getHoleCards()[1]) << std::endl;
	std::cout << showdown(botPlayer.getHoleCards(), oppPlayer.getHoleCards(), boardCards) << std::endl;

//	auto cnode = std::make_shared<ChoiceNode>(0, 0.0, boardCards, botPlayer, oppPlayer, 0, std::shared_ptr<ChoiceNode>(NULL) );
//
//	auto newNode = playTurn(cnode, deck);
//	auto newObj = *newNode;
//	std::cout << "isFolded: " << newObj.getIsFolded() << std::endl;
//	std::cout << "raiseAmount: " << newObj.getCurrentRaise() << std::endl;

	return 0;
}

