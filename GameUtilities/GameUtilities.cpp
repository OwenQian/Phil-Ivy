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
  std::cout << "\nBoard Cards: ";
  for (auto i = boardCards.begin(); i != boardCards.end(); ++i) {
    std::cout << hexToCard(*i) << " ";
  }
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

std::vector<int> deal(std::vector<int> &deck, Stage state) {
  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
  std::vector<int> dealtCards;
  std::shuffle(deck.begin(), deck.end(),
      std::default_random_engine(seed));
  if (state == Stage::HOLECARDS){
    for (int i = 0; i < 2; ++i){
      dealtCards.push_back(deck.back());
      deck.pop_back();
    }
  } else if (state == Stage::PREFLOP) {
    for (int i = 0; i < 3; ++i) {
      dealtCards.push_back(deck.back());
      deck.pop_back();
    }
  } else if (state != Stage::RIVER) {
    dealtCards.push_back(deck.back());
    deck.pop_back();
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

void conditionalDeal(Node& thisNode, Stage refStage, Stage compareStage, std::vector<int> &deck, Stage stage) {
  if (refStage != compareStage) {
    std::vector<int> dealtCards = deal(deck, refStage);
    for (int i: dealtCards)
      thisNode.getGame().getBoardCards().push_back(i);
  } else {
    //std::cout << "comparestage: " << compareStage << std::endl;
    //std::cout << "refstage: " << refStage << std::endl;
  }
}
