#include <vector>
#include <string>
#include <algorithm> 	//for deck shuffling
#include <random>		//for deck shuffling
#include <chrono>		//for deck shuffling seed
#include <iostream>

#include "GameUtilities.h"
#include "helper.h"
#include "../Nodes/ChoiceNode.h"
#include "../Nodes/OpponentNode.h"

static double smallBlind = 25.0;
static double bigBlind = 50.0;
static int smallBlindPosition = 0;

std::vector<int> deal(
						std::vector<int>	&previousDeck,
						int					state) {
	enum states {HOLECARDS, PREFLOP, FLOP, TURN, RIVER};
	unsigned seed =	std::chrono::system_clock::now().time_since_epoch().count();
	std::vector<int> dealtCards;
	std::shuffle(previousDeck.begin(), previousDeck.end(), 
			std::default_random_engine(seed));
	if (state == HOLECARDS){
		for (int i = 0; i < 2; ++i){
			dealtCards.push_back(previousDeck.back());
			previousDeck.pop_back();
		}
	} else if (state == PREFLOP) {
		for (int i = 0; i < 3; ++i) {
			dealtCards.push_back(previousDeck.back());
			previousDeck.pop_back();
		}
	} else if (state != RIVER) {
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

void playGame(){
	double initialChips = 1000.0;
	
	Player botPlayer(0, 0, initialChips, 0); //0, 0 represetn dummy cards
	Player oppPlayer(0, 0, initialChips, 0);
	
	while (botPlayer.getChips() && oppPlayer.getChips()){ // while both players have chips
		std::vector<Player> updatePlayers = playRound (botPlayer, oppPlayer);
		botPlayer = updatePlayers[0];
		oppPlayer = updatePlayers[1];
		smallBlindPosition = ~smallBlindPosition;
	}
}

std::vector<Player> playRound(Player botPlayer, Player oppPlayer){
	enum states {HOLECARDS, PREFLOP, FLOP, TURN, RIVER};
	std::vector<int> deck;
	init_deck(deck);
	botPlayer.setHoleCards(deal(deck, HOLECARDS));
	oppPlayer.setHoleCards(deal(deck, HOLECARDS));
	int currentStage = 0;
	std::shared_ptr<Node> root;
	if (smallBlindPosition == 0){
		root = std::make_shared<ChoiceNode>(0, bigBlind + smallBlind, std::vector<int>(), \
		botPlayer, oppPlayer, smallBlindPosition, std::shared_ptr<ChoiceNode> (NULL));
	} else if (smallBlindPosition == 1){
		root = std::make_shared<OpponentNode>(0, bigBlind + smallBlind, std::vector<int>(), \
		botPlayer, oppPlayer, smallBlindPosition, std::shared_ptr<OpponentNode> (NULL));
	} else {
		std::cout << "fuck owen, also why is smallblindposition not 1 or 0" << std::endl;
		return std::vector<Player>();
	}
	// currentNode infers type of node from root type
	auto currentNode = root;
	while (currentNode != std::shared_ptr<Node>(NULL)) { //might ahve to be shared_ptr null
		currentNode = playTurn(currentNode);
		if (currentStage != (*currentNode).getGame().getState()){
			currentStage++; //goes to the next stage in the game
			std::vector<int> oldBoard = (*currentNode).getGame().getBoardCards();
			std::vector<int> newCards = deal(deck, currentStage);
			//adding current board cards to newly dealt cards
			for (auto i = newCards.begin(); i != newCards.end(); ++i){
				oldBoard.push_back(*i);
			}
			(*currentNode).getGame().setBoardCards(oldBoard);
		}
	}
	// currentNode.getBotPlayer or something like that
	std::vector<Player> updatePlayers{(*currentNode).getGame().getBotPlayer(), (*currentNode).getGame().getOppPlayer() };
	return updatePlayers;
}

std::shared_ptr<ChoiceNode> playTurn(std::shared_ptr<ChoiceNode> currentNode) {
	Decision = bot.decideAction();
	return std::shared_ptr<ChoiceNode>(NULL);
}

std::shared_ptr<OpponentNode> playTurn(std::shared_ptr<OpponentNode> currentNode) {
	return std::shared_ptr<OpponentNode>(NULL);
}

// Won't compile bc AllInNode isn't defined
std::shared_ptr<AllInNode> playTurn(std::shared_ptr<AllInNode> currentNode) {
	return std::shared_ptr<AllInNode>(NULL);
}
