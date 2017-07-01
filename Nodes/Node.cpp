#include <memory>
#include <cassert>
#include <iostream>
#include <chrono>   // Monte carlo timing
#include <cmath>    // naiveUCT log and sqrt
#include <utility>  // for std::pair

#include "Node.h"
#include "ChoiceNode.h"
#include "OpponentNode.h"
#include "../Config.h"
#include "../Stage.h"
#include "../GameUtilities/GameUtilities.h"
#include "../handEval/helper.h"

Node::Node() :
  parent(nullptr),
  game(Stage::HOLECARDS, 0.0, std::vector<int>(), Player(), Player(), 0),
  visitCount(0),
  botExpectedValue(0.0),
  oppExpectedValue(0.0),
  currentRaise(0.0),
  isFolded(false),
  isAllIn(false),
  firstAction(false),
  foldChild(nullptr),
  callChild(nullptr),
  raiseChild(nullptr) { }

Node::Node(Stage             state,
           double            pot,
           std::vector<int>  boardCards,
           Player            botPlayer,
           Player            oppPlayer,
           int               playerTurn,
           Node*             parent,
           int               visitCount,
           double            botExpectedValue,
           double            oppExpectedValue,
           double            currentRaise,
           bool              isFolded,
           bool              isAllIn,
           bool              firstAction) :
  parent(parent),
  game(state, pot, boardCards, botPlayer, oppPlayer, playerTurn),
  visitCount(visitCount),
  botExpectedValue(botExpectedValue),
  oppExpectedValue(oppExpectedValue),
  currentRaise(currentRaise),
  isFolded(isFolded),
  isAllIn(isAllIn),
  firstAction(firstAction),
  foldChild(nullptr),
  callChild(nullptr),
  raiseChild(nullptr) { }

// Copy Constructor
Node::Node(const Node& obj) :
  Node(obj.game.getState(),
       obj.game.getPot(),
       obj.game.getBoardCards(),
       obj.game.getBotPlayer(),
       obj.game.getOppPlayer(),
       obj.game.getPlayerTurn(),
       obj.parent,
       obj.visitCount,
       obj.botExpectedValue,
       obj.oppExpectedValue,
       obj.currentRaise,
       obj.isFolded,
       obj.isAllIn,
       obj.firstAction) { }

Node::Node(const NodeParamObject params) :
  Node(params.state,
      params.pot,
      params.boardCards,
      params.botPlayer,
      params.oppPlayer,
      params.turn,
      params.parent) {}

Node& Node::operator= (const Node& rhs) {
  if (&rhs == this) { return *this; }

  parent = rhs.parent;
  game = rhs.game;
  visitCount = rhs.visitCount;
  botExpectedValue = rhs.botExpectedValue;
  oppExpectedValue = rhs.oppExpectedValue;
  currentRaise = rhs.currentRaise;
  isFolded = rhs.isFolded;
  isAllIn = rhs.isAllIn;
  firstAction = rhs.firstAction;

  return *this;
}

void Node::playGame(){
  Player botPlayer(0, 0, initialChips, 0); //0, 0 represents dummy cards
  Player oppPlayer(0, 0, initialChips, 0);

  while ((botPlayer.getChips() > 0)&& (oppPlayer.getChips() > 0)){
    playRound(botPlayer, oppPlayer);
    smallBlindPosition = !smallBlindPosition;
    botPlayer.setPotInvestment(0);
    oppPlayer.setPotInvestment(0);
  }
}

void Node::playRound(Player& botPlayer, Player& oppPlayer){
  std::cout << "########################################\n";
  std::cout << "smallBlindPosition: " << smallBlindPosition << std::endl;

  // creating the deck
  std::vector<int> deck;
  init_deck(deck);

  // dealing player hole cards
  //const int botCard1 = cardToHex("As"), botCard2 = cardToHex("Kh");
  //const int oppCard1 = cardToHex("Ac"), oppCard2 = cardToHex("Ts");
  //botPlayer.setHoleCards(botCard1, botCard2);
  //oppPlayer.setHoleCards(oppCard1, oppCard2);
  botPlayer.setHoleCards(deal(deck, Stage::HOLECARDS));
  oppPlayer.setHoleCards(deal(deck, Stage::HOLECARDS));

  std::cout << "Bot Cards: " << hexToCard(botPlayer.getHoleCards()[0]) << " " << hexToCard(botPlayer.getHoleCards()[1]);
  std::cout << "\nOpp Cards: " << hexToCard(oppPlayer.getHoleCards()[0]) << " " << hexToCard(oppPlayer.getHoleCards()[1]) << std::endl;

  Stage currentStage = Stage::PREFLOP;

  std::unique_ptr<Node> root;
  Node* currentNode;

  if (smallBlindPosition == 0) {
    root.reset(new ChoiceNode(currentStage, bigBlind + smallBlind, std::vector<int>(),
          botPlayer, oppPlayer, smallBlindPosition, nullptr));
  } else {
    root.reset(new OpponentNode(currentStage, bigBlind + smallBlind, std::vector<int>(),
          botPlayer, oppPlayer, smallBlindPosition, nullptr));
  }

  collectBlinds(root.get());
  root->setIsFirst(true);
  currentNode = root.get();
  printChipSummaries(currentNode->getGame());

  while(!currentNode->getIsAllIn() && !currentNode->getIsFolded()
      && (currentNode->getGame().getState() != Stage::SHOWDOWN)) {
    printCurrentStage(currentStage);
    advanceStage(currentNode, currentStage, deck, currentNode->playTurn(deck));
    printBoardCards(currentNode->getGame().getBoardCards());
    printChipSummaries(currentNode->getGame());
  }
  botPlayer = currentNode->getGame().getBotPlayer();
  oppPlayer = currentNode->getGame().getOppPlayer();
}

// TODO change return type to Action
int Node::playTurn(std::vector<int> deck) {
  assert(!isFolded);
  Decision decision = makeDecision(deck);
  switch(decision.action) {
    case Action::CALL: {
                         call();
                         return 0;
                         break;
                       }
    case Action::RAISE: {
                          raise(decision.raiseAmount);
                          return 1;
                          break;
                        }
    case Action::FOLD: {
                         fold();
                         return 2;
                         break;
                       }
    default:
                       std::cout << "Invalid decision" << std::endl;
  }
}

Action Node::monteCarlo(int maxSeconds, std::vector<int> deck) {
  std::unique_ptr<Node> copyNode;
  if (getGame().getPlayerTurn() == 0) {
    copyNode.reset(new ChoiceNode(*this));
  } else {
    copyNode.reset(new OpponentNode(*this));
  }

  time_t startTime;
  time(&startTime);
  while (time(0) - startTime < maxSeconds) {
    copyNode->runSelection(deck);
  }

  printMonteCarloSummary(copyNode.get());
  return selectBestChild(copyNode.get());
}

void Node::runSelection(std::vector<int> deck) {
  if (isFolded || (game.getState() == Stage::SHOWDOWN) || isAllIn) {
    backprop(game.getBotPlayer().getChips(), game.getOppPlayer().getChips());
    return;
  }

  if (!callChild) {
    Stage currentStage = game.getState();
    call();
    advanceStage(this, currentStage, deck, Action::CALL);
    callChild->runSimulation(deck);
    return;
  } else if (!raiseChild) {
    // TODO Use a different raise amount
    raise(1);
    raiseChild->runSimulation(deck);
    return;
  } else if (!foldChild) {
    fold();
    foldChild->runSimulation(deck);
    return;
  }

  std::vector<double> selectionScores {0,0,0};
  naiveUCT(selectionScores, game.getPlayerTurn());
  double bestScore = 0;

  for (size_t i = 0; i < selectionScores.size(); ++i) {
    bestScore = bestScore > selectionScores[i] ? bestScore : selectionScores[i];
  }
  if (bestScore == selectionScores[0]) {
    if(game.getPlayerTurn() == 1){
    }
    callChild->getGame().getBoardCards() = getGame().getBoardCards();
    conditionalDeal(*callChild, getGame().getState(), callChild->getGame().getState(), deck, getGame().getState());
    callChild->runSelection(deck);
  } else if (bestScore == selectionScores[1]) {
    if(game.getPlayerTurn() == 1){
    }
    raiseChild->getGame().getBoardCards() = getGame().getBoardCards();
    raiseChild->runSelection(deck);
  } else {
    if(game.getPlayerTurn() == 1){
    }
    foldChild->runSelection(deck);
  }
}

void Node::runSimulation(std::vector<int> deck) {
  if (getIsFolded()) {
    allocateChips(!game.getPlayerTurn(), *this);
    backprop(game.getBotPlayer().getChips(), getGame().getOppPlayer().getChips());
    return;
  }

  // if running simulate on a node that called all-in
  if (getIsAllIn()) {
    for (Stage i = getGame().getState(); i != Stage::SHOWDOWN; ++i) {
      std::vector<int> tempDealt = deal(deck, i);
      for (int j:tempDealt) {
        game.getBoardCards().push_back(j);
      }
    }
    int winner = showdown(game.getBotPlayer().getHoleCards(), game.getOppPlayer().getHoleCards(), game.getBoardCards());
    allocateChips(winner, *this);
    backprop(game.getBotPlayer().getChips(), getGame().getOppPlayer().getChips());
    return;
  }

  std::unique_ptr<Node> rootNode;
  Node* currentNode;

  if (game.getPlayerTurn() == 0){
    rootNode.reset(new ChoiceNode(*this));
  } else {
    rootNode.reset(new OpponentNode(*this));
  }

  currentNode = rootNode.get();
  Stage prevStage;
  while (currentNode->getGame().getState() != Stage::SHOWDOWN) {
    prevStage = currentNode->getGame().getState();
    currentNode->call();
    currentNode = currentNode->callChild.get();
    conditionalDeal(*currentNode, prevStage, currentNode->getGame().getState(), deck, Stage::PREFLOP);
    if (currentNode->getIsAllIn()) {
      for (Stage i = currentNode->getGame().getState(); i != Stage::SHOWDOWN; ++i) {
        std::vector<int> tempDealt = deal(deck, i);
        for (int j:tempDealt) {
          currentNode->getGame().getBoardCards().push_back(j); //used to be conditionalDeal
        }
      }
      break;
    } 
  }
  if (currentNode->getGame().getBoardCards().size() < 5) {
  }

  int winner = showdown( 
      currentNode->getGame().getBotPlayer().getHoleCards(),
      currentNode->getGame().getOppPlayer().getHoleCards(),
      currentNode->getGame().getBoardCards());
  allocateChips(winner, *currentNode);
  currentNode->backprop(currentNode->getGame().getBotPlayer().getChips(),
      currentNode->getGame().getOppPlayer().getChips());
}

void Node::backprop(double botChips, double oppChips) {
  getBotExpectedValue() = (getBotExpectedValue() * visitCount + botChips) / (visitCount + 1);
  getOppExpectedValue() = (getOppExpectedValue() * visitCount + oppChips) / (visitCount + 1);
  ++visitCount;
  if (parent != nullptr) {
    parent->backprop(botChips, oppChips);
  }
}

void Node::naiveUCT(std::vector<double>& selectionScores, int playerTurn) {
  assert(selectionScores.size() == 3);
  std::vector<double> explorationTerm(3, 0);

  // use bot or opp EV based on pTurn
  std::vector<double> ambiguousPlayerEV(3, 0);
  if (playerTurn == 0) {
    ambiguousPlayerEV[0] = callChild->getBotExpectedValue();
    ambiguousPlayerEV[1] = raiseChild->getBotExpectedValue();
    ambiguousPlayerEV[2] = foldChild->getBotExpectedValue();
  } else {
    ambiguousPlayerEV[0] = callChild->getOppExpectedValue();
    ambiguousPlayerEV[1] = raiseChild->getOppExpectedValue();
    ambiguousPlayerEV[2] = foldChild->getOppExpectedValue();
  }

  // Order here is important; call, raise, fold (CRF)
  // Set the selectionScore and explorationTerm for call
  selectionScores[0] = ambiguousPlayerEV[0];
  explorationTerm[0] = calculateNaiveUCT(callChild->visitCount, visitCount);

  // Set the selectionScore and explorationTerm for raise
  selectionScores[1] = ambiguousPlayerEV[1];
  explorationTerm[1] = calculateNaiveUCT(raiseChild->visitCount, visitCount);

  // Set the selectionScore and explorationTerm for fold
  selectionScores[2] = ambiguousPlayerEV[2];
  explorationTerm[2] = calculateNaiveUCT(foldChild->visitCount, visitCount);

  for (size_t i = 0; i < selectionScores.size(); ++i) {
    explorationTerm[i] *= exploreConst;
    selectionScores[i] += explorationTerm[i];
  }
}

bool Node::isAllInCheck(Player p1, Player p2) {
  return (p1.getChips() + p2.getPotInvestment() <= currentRaise ||
      p2.getChips() + p2.getPotInvestment() <= currentRaise);
}

Node* Node::getChildNode(int n) {
  switch(n) {
    case 0:
      return callChild.get();
      break;
    case 1:
      return raiseChild.get();
      break;
    case 2:
      return foldChild.get();
      break;
    default:
      std::cout << "ERROR: Invalid ChildNode idx - getChildNode()" << std::endl;
      return nullptr;
  }
}

void Node::call() {
  Player botPlayer = game.getBotPlayer();
  Player oppPlayer = game.getOppPlayer();
  Player* currentP = game.getPlayerTurn() ? &oppPlayer : &botPlayer;
  Player* otherP = !game.getPlayerTurn() ? &oppPlayer : &botPlayer;

  currentP->setChips(currentP->getChips() - (currentRaise - currentP->getPotInvestment()));
  currentP->setPotInvestment(currentRaise);
  int turn = getIsFirst() ? !game.getPlayerTurn() : !smallBlindPosition;
  double potFromInitialChips = initialChips * 2 - currentP->getChips() - otherP->getChips();
  NodeParamObject nodeParams(game.getState() + !getIsFirst(),
      potFromInitialChips,
      game.getBoardCards(),
      botPlayer,
      oppPlayer,
      turn,
      this);
  if (turn == 1) {
    callChild.reset(new OpponentNode(nodeParams));
  } else {
    callChild.reset(new ChoiceNode(nodeParams));
  }
  callChild->setIsAllIn(isAllInCheck(botPlayer, oppPlayer));
  callChild->setCurrentRaise(firstAction * currentRaise);
  // Reset pot investment if it's not the first action
  double newPotInvestment = firstAction * callChild->getGame().getBotPlayer().getPotInvestment();
  callChild->getGame().getBotPlayer().setPotInvestment(newPotInvestment);
  callChild->getGame().getOppPlayer().setPotInvestment(newPotInvestment);
  callChild->setIsFirst(!firstAction);
}

void Node::raise(double raiseAmount) {
  Player botPlayer = game.getBotPlayer();
  Player oppPlayer = game.getOppPlayer();
  if (isAllInCheck(botPlayer, oppPlayer)) {
    call();
    if (callChild->getGame().getPlayerTurn() == 0) {
      raiseChild.reset(new ChoiceNode(*callChild.get()));
    } else {
      raiseChild.reset(new OpponentNode(*callChild.get()));
    }
    return;
  }

  setIsFirst(false);

  Player* currentP = game.getPlayerTurn() ? &oppPlayer : &botPlayer;
  Player* otherP = !game.getPlayerTurn() ? &oppPlayer : &botPlayer;
  double effectiveStack = std::min((currentP->getChips() + currentP->getPotInvestment()),
      (otherP->getChips() + otherP->getPotInvestment()));
  raiseAmount = std::min(effectiveStack, std::max(std::max(bigBlind, raiseAmount), 2*currentRaise));

  currentP->setChips(currentP->getChips() - (raiseAmount - currentP->getPotInvestment()));
  currentP->setPotInvestment(raiseAmount);
  int turn = !game.getPlayerTurn();
  double potFromInitialChips = initialChips * 2 - currentP->getChips() - otherP->getChips();
  NodeParamObject nodeParams(game.getState(),
      potFromInitialChips,
      game.getBoardCards(),
      botPlayer,
      oppPlayer,
      turn,
      this);
  if (turn == 1) {
    raiseChild.reset(new OpponentNode(nodeParams));
  } else {
    raiseChild.reset(new ChoiceNode(nodeParams));
  }
  raiseChild->setCurrentRaise(raiseAmount);
}

void Node::fold() {
  if (game.getPlayerTurn() == 0) {
    foldChild.reset(new OpponentNode(*this));
  } else {
    foldChild.reset(new ChoiceNode(*this));
  }
  foldChild->setParent(this);
  foldChild->setIsFolded(true);
  foldChild->setVisitCount(0);
  allocateChips(!game.getPlayerTurn(), *foldChild.get());
}
