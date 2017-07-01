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

void printCurrentStage(Stage stage) {
  std::string s;
  switch (stage) {
    case Stage::PREFLOP:
      s = "PREFLOP";
      break;
    case Stage::FLOP:
      s = "FLOP";
      break;
    case Stage::TURN:
      s = "TURN";
      break;
    case Stage::RIVER:
      s = "RIVER";
        break;
    default:
      s = "INVALID STAGE";
  }
  std::cout << "\nCurrent Stage: " << s << std::endl;
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

void updateBoard(Node* node, Stage stage, std::vector<int>& deck) {
  std::vector<int> newBoard = node->getGame().getBoardCards();
  std::vector<int> newCards = deal(deck, stage);

  assert(newCards.size() <= 3);
  for (auto i = newCards.begin(); i != newCards.end(); ++i) {
    newBoard.push_back(*i);
  }
  node->getGame().setBoardCards(newBoard);
  assert(node->getGame().getBoardCards().size() <= 5);
}

void advanceStage(Node*& node, Stage& refStage, std::vector<int>& deck, int action) {
  node = node->getChildNode(action);
  if (node->getGame().getState() != refStage) {
    node->setIsFirst(true);
    updateBoard(node, refStage++, deck);
  }
  if (node->getGame().getState() == Stage::SHOWDOWN) {
    handleShowdown(node);
  }
  if (node->getIsAllIn()) {
    handleAllIn(node, deck);
  }
}

void collectBlinds(Node* node) {
  Player botPlayer = node->getGame().getBotPlayer();
  Player oppPlayer = node->getGame().getOppPlayer();
  Player* smallBlindPlayer = smallBlindPosition ? &oppPlayer : &botPlayer;
  double smallBlindPayed = std::min(smallBlindPlayer->getChips(), smallBlind);
  smallBlindPlayer->setChips(smallBlindPlayer->getChips() - smallBlindPayed);
  smallBlindPlayer->setPotInvestment(smallBlindPayed);

  Player* bigBlindPlayer = !smallBlindPosition ? &oppPlayer : &botPlayer;
  double bigBlindPayed = std::min(bigBlindPlayer->getChips(), bigBlind);
  bigBlindPlayer->setChips(bigBlindPlayer->getChips() - bigBlindPayed);
  bigBlindPlayer->setPotInvestment(bigBlindPayed);
  node->getGame().setBotPlayer(botPlayer);
  node->getGame().setOppPlayer(oppPlayer);

  node->setCurrentRaise(bigBlindPayed);
}

void handleAllIn(Node* node, std::vector<int>& deck) {
  for (Stage s = node->getGame().getState(); s < Stage::SHOWDOWN; ++s) {
    updateBoard(node, s, deck);
  }
  handleShowdown(node);
}

void handleShowdown(Node* node) {
    int winner = showdown(node->getGame().getBotPlayer().getHoleCards(),
        node->getGame().getOppPlayer().getHoleCards(),
        node->getGame().getBoardCards());
    allocateChips(winner, *node);
    std::string s = !winner ? "BOT" : "OPP";
    if (winner == 2) { s = "CHOP"; }
    std::cout << "\nResult: " << s << std::endl;
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

void printChipSummaries(const GameObject& game) {
  std::cout << "\nbot player chips: " << game.getBotPlayer().getChips() <<
    ", potInvestment: " << game.getBotPlayer().getPotInvestment() << std::endl;
  std::cout << "opp player chips: " << game.getOppPlayer().getChips() <<
    ", potInvestment: " << game.getOppPlayer().getPotInvestment() << std::endl;
  std::cout << "Pot: " << game.getPot() << std::endl;
}

void printMonteCarloSummary(Node* copyNode) {
  std::cout << "visitCount: " << copyNode->getVisitCount();

  std::cout << "\n\n@@callVisit: " << copyNode->callChild->getVisitCount();
  std::cout << "\n bot callScore: " << copyNode->callChild->getBotExpectedValue();
  std::cout << "\n opp callScore: " << copyNode->callChild->getOppExpectedValue();

  std::cout << "\n\n@@raiseVisit: " << copyNode->raiseChild->getVisitCount();
  std::cout << "\n bot raiseScore: " << copyNode->raiseChild->getBotExpectedValue();
  std::cout << "\n opp raiseScore: " << copyNode->raiseChild->getOppExpectedValue();

  std::cout << "\n\n@@foldVisit: " << copyNode->foldChild->getVisitCount();
  std::cout << "\n bot foldScore: " << copyNode->foldChild->getBotExpectedValue();
  std::cout << "\n opp foldScore: " << copyNode->foldChild->getOppExpectedValue() << std::endl;
}

Action selectBestChild(Node* copyNode) {
  std::pair<double, int> callEV = std::make_pair(copyNode->callChild->getBotExpectedValue(), 0);
  std::pair<double, int> raiseEV = std::make_pair(copyNode->raiseChild->getBotExpectedValue(), 1);
  std::pair<double, int> foldEV = std::make_pair(copyNode->foldChild->getBotExpectedValue(), 2);
  std::vector<std::pair<double, int> >expectedValues {callEV, raiseEV, foldEV};
  std::sort(expectedValues.begin(), expectedValues.end());
  int selectedIdx = expectedValues[2].second;

  switch (selectedIdx) {
    case 0:
      return Action::CALL;
      break;
    case 1:
      return Action::RAISE;
      break;
    case 2:
      return Action::FOLD;
      break;
    default:
      std::cout << "\nINVALID SELECT IDX IN monteCarlo() and selectBestChild()" << std::endl;
      return Action::FOLD;
  }
}

double calculateNaiveUCT(int nodeVisit, int totalVisit) {
    return std::sqrt(std::log(double (totalVisit) / double (nodeVisit));
}
