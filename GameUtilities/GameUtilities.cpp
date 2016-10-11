#include <vector>
#include <cassert>
#include <string>
#include <algorithm> 	//for deck shuffling
#include <random>		//for deck shuffling
#include <chrono>		//for deck shuffling seed
#include <iostream>

#include "../Nodes/Node.h"
#include "GameUtilities.h"
#include "../handEval/helper.h"
#include "../Stage.h"
#include "../Action.h"
#include "Decision.h"
#include "../Config.h"

void printBoardCards(std::vector<int> boardCards) {
    std::cout << "Board Cards: ";
    for (auto i = boardCards.begin(); i != boardCards.end(); ++i)
        std::cout << hexToCard(*i) << " ";
    //std::cout << std::endl;
}

void allocateChips(int whoWon, Node &currentNode){
	if (whoWon == 0) { //when bot player wins
		currentNode.getGame().getBotPlayer().addChips(currentNode.getGame().getPot());
	} else if (whoWon == 1) { // when opp player wins
		currentNode.getGame().getOppPlayer().addChips(currentNode.getGame().getPot());
	} else { //tie
		currentNode.getGame().getOppPlayer().addChips(currentNode.getGame().getPot()/2);
		currentNode.getGame().getBotPlayer().addChips(currentNode.getGame().getPot()/2);
	}
}

std::vector<int> deal(
		std::vector<int>	&previousDeck,
		int					state) {
	unsigned seed =	std::chrono::system_clock::now().time_since_epoch().count();
	std::vector<int> dealtCards;
	std::shuffle(previousDeck.begin(), previousDeck.end(), 
			std::default_random_engine(seed));
	if (state == static_cast<int>(Stage::HOLECARDS) ){
		for (int i = 0; i < 2; ++i){
			dealtCards.push_back(previousDeck.back());
			previousDeck.pop_back();
		}
	} else if (state == static_cast<int>(Stage::PREFLOP) ) {
		for (int i = 0; i < 3; ++i) {
			dealtCards.push_back(previousDeck.back());
			previousDeck.pop_back();
		}
	} else if (state != static_cast<int>(Stage::RIVER) ) {
		dealtCards.push_back(previousDeck.back());
		previousDeck.pop_back();
	}
	return dealtCards;
}

void init_deck(std::vector<int>& deck) {
	deck.reserve(52);
	const char ranks[] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
	const char suits[] = {'s', 'h', 'd', 'c'};
	for (char i: ranks) {
		for (char j: suits) {
			std::string s {i, j};
			deck.push_back(cardToHex(s));
		}
	}
}

void conditionalDeal(Node& thisNode, int refStage, int compareStage, std::vector<int> &deck, int stage) {
    if (refStage != compareStage) {
        std::vector<int> dealtCards = deal(deck, stage);
        for (int i: dealtCards)
            thisNode.getGame().getBoardCards().push_back(i);
    }
}
