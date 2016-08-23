#include <vector>
#include <cassert>
#include <string>
#include <algorithm> 	//for deck shuffling
#include <random>		//for deck shuffling
#include <chrono>		//for deck shuffling seed
#include <iostream>

#include "GameUtilities.h"
#include "../handEval/helper.h"
#include "../Nodes/ChoiceNode.h"
#include "../Nodes/OpponentNode.h"
#include "../Stage.h"
#include "Action.h"
#include "Decision.h"
#include "../Config.h"

class Node;
class ChoiceNode;
class OpponentNode;

void printBoardCards(std::vector<int> boardCards) {
    std::cout << "Board Cards: ";
    for (auto i = boardCards.begin(); i != boardCards.end(); ++i)
        std::cout << hexToCard(*i) << " ";
    std::cout << std::endl;
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

void playGame(){
	Player botPlayer(0, 0, initialChips, 0); //0, 0 represents dummy cards
	Player oppPlayer(0, 0, initialChips, 0);

	while ((botPlayer.getChips() > 0)&& (oppPlayer.getChips() > 0)){ // while both players have chips
		std::vector<Player> updatePlayers = playRound (botPlayer, oppPlayer);
		botPlayer = updatePlayers[0];
		oppPlayer = updatePlayers[1];
		std::cout << "bot player chips: " << botPlayer.getChips() << std::endl;
		std::cout << "opp player chips: " << oppPlayer.getChips() << std::endl;
		smallBlindPosition = !smallBlindPosition;
        botPlayer.setPotInvestment(0);
        oppPlayer.setPotInvestment(0);
	}
}

std::vector<Player> playRound(Player botPlayer, Player oppPlayer){
	std::cout << "########################################" << std::endl;
	std::cout << "Small Blind: " << smallBlind << "\nBig Blind : " << bigBlind << std::endl;
	std::cout << "bot player chips: " << botPlayer.getChips() << std::endl;
	std::cout << "opp player chips: " << oppPlayer.getChips() << std::endl;

    // creating the deck
	std::vector<int> deck;
	init_deck(deck);

    // dealing player hole cards
	botPlayer.setHoleCards(deal(deck, static_cast<int>(Stage::HOLECARDS) ));
	oppPlayer.setHoleCards(deal(deck, static_cast<int>(Stage::HOLECARDS) ));

	int currentStage = 1;       // preflop stage

	std::shared_ptr<Node> root;
	std::cout << "smallblindposition: " << smallBlindPosition << std::endl;
    // if smallBlindPosition == 0, node should be a ChoiceNode
	assert(smallBlindPosition == 0 || smallBlindPosition == 1);
	if (smallBlindPosition == 0){
		root = std::make_shared<ChoiceNode>(1, bigBlind + smallBlind, std::vector<int>(),
				botPlayer, oppPlayer, smallBlindPosition, std::shared_ptr<ChoiceNode> (NULL));
		// if the smallBlind puts the bot allIn
		if (smallBlind >= botPlayer.getChips()){
			root->getGame().getOppPlayer().setChips(oppPlayer.getChips() - botPlayer.getChips());
			root->getGame().getBotPlayer().setChips(0);
			root->getGame().getBotPlayer().setPotInvestment(botPlayer.getChips());
			root->getGame().getOppPlayer().setPotInvestment(botPlayer.getChips());
        // if the bigBlind puts the opp alIn
		} else if (bigBlind >= oppPlayer.getChips()) {
			double lesserAmount = smallBlind < oppPlayer.getChips() ? smallBlind : oppPlayer.getChips();
			root->getGame().getBotPlayer().setChips(botPlayer.getChips() - lesserAmount);
			root->getGame().getOppPlayer().setChips(0);
			root->getGame().getBotPlayer().setPotInvestment(lesserAmount);
			root->getGame().getOppPlayer().setPotInvestment(oppPlayer.getChips());
			root->setCurrentRaise(lesserAmount);
		} else {
			// subtract blinds from chip count
			root->getGame().getBotPlayer().setChips(botPlayer.getChips() - smallBlind);
			root->getGame().getOppPlayer().setChips(oppPlayer.getChips() - bigBlind);
			// add blinds to potInvestment
			root->getGame().getBotPlayer().setPotInvestment(smallBlind);
			root->getGame().getOppPlayer().setPotInvestment(bigBlind);
			root->setCurrentRaise(bigBlind);
		}
	} else if (smallBlindPosition == 1) {
		root = std::make_shared<OpponentNode>(1, bigBlind + smallBlind, std::vector<int>(),
				botPlayer, oppPlayer, smallBlindPosition, std::shared_ptr<OpponentNode> (NULL));
		// if the smallBlind puts the opp allIn
		if (smallBlind >= oppPlayer.getChips()){
			root->getGame().getBotPlayer().setChips(botPlayer.getChips() - oppPlayer.getChips());
			root->getGame().getOppPlayer().setChips(0);
			root->getGame().getBotPlayer().setPotInvestment(oppPlayer.getChips());
			root->getGame().getOppPlayer().setPotInvestment(oppPlayer.getChips());
        // if the bigBlind puts the bot alIn
		} else if (bigBlind >= botPlayer.getChips()) {
			double lesserAmount = smallBlind < botPlayer.getChips() ? smallBlind : botPlayer.getChips();
			root->getGame().getOppPlayer().setChips(oppPlayer.getChips() - lesserAmount);
			root->getGame().getBotPlayer().setChips(0);
			root->getGame().getOppPlayer().setPotInvestment(lesserAmount);
			root->getGame().getBotPlayer().setPotInvestment(botPlayer.getChips());
			root->setCurrentRaise(lesserAmount);
		} else {
			// subtract blinds from chip count
			root->getGame().getBotPlayer().setChips(botPlayer.getChips() - bigBlind);
			root->getGame().getOppPlayer().setChips(oppPlayer.getChips() - smallBlind);

			// add blinds to potInvestment
			root->getGame().getBotPlayer().setPotInvestment(bigBlind);
			root->getGame().getOppPlayer().setPotInvestment(smallBlind);
			root->setCurrentRaise(bigBlind);
		}
	} 
	// currentNode infers type of node from root type
    std::shared_ptr<Node> currentNode = root;
	currentNode->setIsFirst(true);
	while (currentNode->getIsFolded() == 0) { 
		if (currentNode->getGame().getPlayerTurn() == 0 && currentNode->getIsAllIn() == false)
			currentNode = playTurn(std::static_pointer_cast<ChoiceNode>(currentNode), deck);
		else if (currentNode->getIsAllIn() == false)
			currentNode = playTurn(std::static_pointer_cast<OpponentNode>(currentNode), deck);

        // deal cards if state's changes or we're all in
		if (currentStage != currentNode->getGame().getState() || currentNode->getIsAllIn()){
			std::vector<int> updateBoard = currentNode->getGame().getBoardCards();
			std::vector<int> newCards = deal(deck, currentStage);
			currentStage++;
			//adding current board cards to newly dealt cards
			assert(newCards.size() <= 3);
			for (auto i = newCards.begin(); i != newCards.end(); ++i){
				updateBoard.push_back(*i);
			}
            printBoardCards(updateBoard);
			assert(updateBoard.size() <= 5);
			currentNode->getGame().setBoardCards(updateBoard);
			if (currentNode->getIsAllIn()) {
				for (int i = currentNode->getGame().getState() + 1; i < static_cast<int>(Stage::SHOWDOWN); ++i) {
					std::vector<int> updateBoard = currentNode->getGame().getBoardCards();
					std::vector<int> newCards = deal(deck, i);
					//adding current board cards to newly dealt cards
					assert(newCards.size() <= 3);
					for (auto j = newCards.begin(); j != newCards.end(); ++j){
						updateBoard.push_back(*j);
					}
					assert(updateBoard.size() <=5);
					currentNode->getGame().setBoardCards(updateBoard);
				}
                printBoardCards(currentNode->getGame().getBoardCards());
				std::cout << "botCards: " << hexToCard(currentNode->getGame().getBotPlayer().getHoleCards()[0]) << " " << hexToCard(currentNode->getGame().getBotPlayer().getHoleCards()[1]) << std::endl;
				std::cout << "oppCards: " << hexToCard(currentNode->getGame().getOppPlayer().getHoleCards()[0]) << " " << hexToCard(currentNode->getGame().getOppPlayer().getHoleCards()[1]) << std::endl;
				int winner = showdown(currentNode->getGame().getBotPlayer().getHoleCards(),
						currentNode->getGame().getOppPlayer().getHoleCards(),
						currentNode->getGame().getBoardCards());
				allocateChips(winner, (*currentNode));
				std::cout << "Winner: " << winner << std::endl;
				currentNode = std::static_pointer_cast<ChoiceNode>(currentNode)->fold();
			}

			currentNode->setIsFirst(true);
			currentNode->getGame().setPlayerTurn(smallBlindPosition);
		}
	}
	// currentNode.getBotPlayer or something like that
	std::vector<Player> updatePlayers{currentNode->getGame().getBotPlayer(), currentNode->getGame().getOppPlayer()};
	return updatePlayers;
}

std::shared_ptr<OpponentNode> playTurn(std::shared_ptr<ChoiceNode> currentNode, std::vector<int> deck) {
	int stageInt = currentNode->getGame().getState();
	assert(currentNode->getIsFolded() == false);
	if (stageInt != static_cast<int>(Stage::SHOWDOWN)) {
		Decision decision = Decision::makeDecision(currentNode);
		switch(decision.action) {
			case Action::CALL: {
								   auto returnNode = currentNode->call();
								   return returnNode;
								   break; 
							   }
			case Action::RAISE:{
								   auto returnNode = currentNode->raise(decision.raiseAmount);
								   return returnNode;
								   break;
							   }
			case Action::FOLD:{
								  allocateChips(1, (*currentNode));
								  auto returnNode = currentNode->fold();
								  return returnNode;
								  break;
							  }
			default:
							  std::cout << "Invalid action" << std::endl;
		}
	} else if (stageInt == static_cast<int> (Stage::SHOWDOWN)) {
		int winner = showdown(currentNode->getGame().getBotPlayer().getHoleCards(),
				currentNode->getGame().getOppPlayer().getHoleCards(),
				currentNode->getGame().getBoardCards());
		allocateChips(winner, (*currentNode));
		auto returnNode = currentNode->fold();
		return returnNode;
	} 
	return std::shared_ptr<OpponentNode>(NULL);
}

std::shared_ptr<ChoiceNode> playTurn(std::shared_ptr<OpponentNode> currentNode, std::vector<int> deck) {
	int stateInt = currentNode->getGame().getState();
	assert(currentNode->getIsFolded() == false);
	if (stateInt != static_cast<int>(Stage::SHOWDOWN)) {
		Decision decision = Decision::makeDecision(currentNode);
		switch(decision.action) {
			case Action::CALL: {
								   auto returnNode = currentNode->call();
								   return returnNode;
								   break; 
							   }
			case Action::RAISE:{
								   auto returnNode = currentNode->raise(decision.raiseAmount);
								   return returnNode;
								   break;
							   }
			case Action::FOLD:{
								  allocateChips(0, (*currentNode));
								  auto returnNode = currentNode->fold();
								  return returnNode;
								  break;
							  }
			default:
							  std::cout << "Invalid action" << std::endl;
		}
	} else if (stateInt == static_cast<int> (Stage::SHOWDOWN)) {
		int winner = showdown(currentNode->getGame().getBotPlayer().getHoleCards(),
				currentNode->getGame().getOppPlayer().getHoleCards(),
				currentNode->getGame().getBoardCards());
		allocateChips(winner, (*currentNode));
		auto returnNode = currentNode->fold();
		return returnNode;
	} 
	return std::shared_ptr<ChoiceNode>(NULL);
}

void conditionalDeal(Node& thisNode, int refStage, int compareStage, std::vector<int> deck, int stage) {
    if (refStage != compareStage) {
        std::vector<int> dealtCards = deal(deck, stage);
        for (int i: dealtCards)
            thisNode.getGame().getBoardCards().push_back(i);
    }
}
