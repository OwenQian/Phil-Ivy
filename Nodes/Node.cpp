#include <memory>
#include <typeinfo>
#include <utility>
#include <cassert>
#include <iostream>
#include <chrono>   // Monte carlo timing
#include <cmath>    // naiveUCT log and sqrt

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
             int    visitCount,
             double botExpectedValue,
             double oppExpectedValue,
             double currentRaise,
             bool   isFolded,
             bool   isAllIn,
             bool   firstAction) :
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

// Destructor
Node::~Node() {
}

// Assignment operaor
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
  const int botCard1 = cardToHex("As"), botCard2 = cardToHex("Kh");
  const int oppCard1 = cardToHex("Ac"), oppCard2 = cardToHex("Ts");
  botPlayer.setHoleCards(botCard1, botCard2);
  oppPlayer.setHoleCards(oppCard1, oppCard2);
  //botPlayer.setHoleCards(deal(deck, static_cast<int>(Stage::HOLECARDS)));
  //oppPlayer.setHoleCards(deal(deck, static_cast<int>(Stage::HOLECARDS)));

  std::cout << "Bot Cards: " << hexToCard(botPlayer.getHoleCards()[0]) << " " << hexToCard(botPlayer.getHoleCards()[1]);
  std::cout << "\nOpp Cards: " << hexToCard(oppPlayer.getHoleCards()[0]) << " " << hexToCard(oppPlayer.getHoleCards()[1]) << std::endl;

  Stage currentStage = Stage::PREFLOP;

  std::unique_ptr<Node> root;
  Node* currentNode;

  if (smallBlindPosition == 0) {
    root.reset( new ChoiceNode(currentStage, bigBlind + smallBlind, std::vector<int>(),
          botPlayer, oppPlayer, smallBlindPosition, nullptr));
  } else {
    root.reset( new OpponentNode(currentStage, bigBlind + smallBlind, std::vector<int>(),
          botPlayer, oppPlayer, smallBlindPosition, nullptr) );
  }

  root->collectBlinds();
  root->setIsFirst(true);
  currentNode = root.get();
  std::cout << "bot player chips: " << currentNode->getGame().getBotPlayer().getChips() << std::endl;
  std::cout << "bot player chips: " << currentNode->getGame().getOppPlayer().getChips() << std::endl;
  std::cout << "Pot: " << currentNode->getGame().getPot() << std::endl;

  while(!currentNode->getIsAllIn() && !currentNode->getIsFolded()
      && (currentNode->getGame().getState() != Stage::SHOWDOWN)) {
    printCurrentStage(currentStage);
    currentNode->advanceStage(currentStage, deck, currentNode->playTurn(deck));
    std::cout << "Pot: " << currentNode->game.getPot() << std::endl;
    std::cout << "BotChips: " << currentNode->game.getBotPlayer().getChips() << std::endl;
    std::cout << "OppChips: " << currentNode->game.getOppPlayer().getChips() << std::endl;
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

  std::cout << "visitCount: " << copyNode->visitCount;

  std::cout << "\n\n@@callVisit: " << copyNode->callChild->visitCount;
  std::cout << "\n bot callScore: " << copyNode->callChild->getBotExpectedValue();
  std::cout << "\n opp callScore: " << copyNode->callChild->getOppExpectedValue();

  std::cout << "\n\n@@raiseVisit: " << copyNode->raiseChild->visitCount;
  std::cout << "\n bot raiseScore: " << copyNode->raiseChild->getBotExpectedValue();
  std::cout << "\n opp raiseScore: " << copyNode->raiseChild->getOppExpectedValue();

  std::cout << "\n\n@@foldVisit: " << copyNode->foldChild->visitCount;
  std::cout << "\n bot foldScore: " << copyNode->foldChild->getBotExpectedValue();
  std::cout << "\n opp foldScore: " << copyNode->foldChild->getOppExpectedValue() << std::endl;

  double maxScore = copyNode->callChild->getBotExpectedValue();
  maxScore = maxScore >= copyNode->raiseChild->getBotExpectedValue() ? maxScore : copyNode->raiseChild->getBotExpectedValue();
  maxScore = maxScore >= copyNode->foldChild->getBotExpectedValue() ? maxScore : copyNode->foldChild->getBotExpectedValue();
  if (maxScore == copyNode->callChild->getBotExpectedValue()) {
    return Action::CALL;
  } else if (maxScore == copyNode->raiseChild->getBotExpectedValue()) {
    // Need to handle how much to raise here
    return Action::RAISE;
  } else {
    return Action::FOLD;
  }
}

void Node::runSelection(std::vector<int> deck) {
  if (isFolded || (game.getState() == Stage::SHOWDOWN) || isAllIn) {
    backprop(game.getBotPlayer().getChips(), game.getOppPlayer().getChips());
    return;
  }

  if (!callChild) {
    call();
    conditionalDeal(*callChild, getGame().getState(), callChild->getGame().getState(), deck, getGame().getState());
    //issue with dealing exists here as well
    callChild->runSimulation(deck);
    if (!(callChild->callChild) && !(callChild->raiseChild) && !(callChild->foldChild)) {
    }

    return;
  } else if (!raiseChild) {
    // TODO Use a different raise amount
    raise(1);
    conditionalDeal(*raiseChild, getGame().getState(), raiseChild->getGame().getState(), deck, Stage::PREFLOP);
    conditionalDeal(*callChild, getGame().getState(), callChild->getGame().getState(), deck, Stage::PREFLOP);
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
  if (2000 - epsilon > getBotExpectedValue() + getOppExpectedValue() || 2000 + epsilon < getBotExpectedValue() + getOppExpectedValue()){
  }
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
  explorationTerm[0] = std::sqrt( std::log(double (visitCount)) 
      / double (callChild->visitCount));

  // Set the selectionScore and explorationTerm for raise
  selectionScores[1] = ambiguousPlayerEV[1];
  explorationTerm[1] = std::sqrt( std::log(double (visitCount))
      / double (raiseChild->visitCount));

  // Set the selectionScore and explorationTerm for fold
  selectionScores[2] = ambiguousPlayerEV[2];
  explorationTerm[2] = std::sqrt( std::log( double(visitCount) )
      / double (foldChild->visitCount) );

  for (size_t i = 0; i < selectionScores.size(); ++i) {
    explorationTerm[i] *= exploreConst;
    selectionScores[i] += explorationTerm[i]; 
  }
}

bool Node::isAllInCheck(Player p1, Player p2) {
  return (p1.getChips() + p2.getPotInvestment() <= currentRaise ||
      p2.getChips() + p2.getPotInvestment() <= currentRaise);
}

void Node::collectBlinds() {
  Player botPlayer = game.getBotPlayer();
  Player oppPlayer = game.getOppPlayer();
  Player* smallBlindPlayer = smallBlindPosition ? &oppPlayer : &botPlayer;
  double smallBlindPayed = std::min(smallBlindPlayer->getChips(), smallBlind);
  smallBlindPlayer->setChips(smallBlindPlayer->getChips() - smallBlindPayed);
  smallBlindPlayer->setPotInvestment(smallBlindPayed);

  Player* bigBlindPlayer = !smallBlindPosition ? &oppPlayer : &botPlayer;
  double bigBlindPayed = std::min(bigBlindPlayer->getChips(), bigBlind);
  bigBlindPlayer->setChips(bigBlindPlayer->getChips() - bigBlindPayed);
  bigBlindPlayer->setPotInvestment(bigBlindPayed);
  game.setBotPlayer(botPlayer);
  game.setOppPlayer(oppPlayer);

  setCurrentRaise(bigBlindPayed);
}

void Node::handleShowdown() {
    int winner = showdown(getGame().getBotPlayer().getHoleCards(),
        getGame().getOppPlayer().getHoleCards(),
        getGame().getBoardCards());
    allocateChips(winner, *this);
    std::string s = !winner ? "BOT" : "OPP";
    if (winner == 2) { s = "CHOP"; }
    std::cout << "Result: " << s << std::endl;
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
void Node::updateBoard(Stage stage, std::vector<int>& deck) {
  std::vector<int> newBoard = getGame().getBoardCards();
  std::vector<int> newCards = deal(deck, stage);

  assert(newCards.size() <= 3);
  for (auto i = newCards.begin(); i != newCards.end(); ++i){
    newBoard.push_back(*i);
  }
  getGame().setBoardCards(newBoard);
  printBoardCards(newBoard);
  assert(getGame().getBoardCards().size() <= 5);
}

void Node::printCurrentStage(Stage stage) {
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

void Node::call() {
  Player botPlayer = game.getBotPlayer();
  Player oppPlayer = game.getOppPlayer();
  Player* currentP = game.getPlayerTurn() ? &oppPlayer : &botPlayer;
  Player* otherP = !game.getPlayerTurn() ? &oppPlayer : &botPlayer;

  currentP->setChips(currentP->getChips() - (currentRaise - currentP->getPotInvestment()));
  currentP->setPotInvestment(currentRaise);
  int turn = getIsFirst() ? !game.getPlayerTurn() : smallBlindPosition;
  NodeParamObject nodeParams(game.getState() + !getIsFirst(),
      initialChips * 2 - currentP->getChips() - otherP->getChips(),
      game.getBoardCards(),
      botPlayer,
      oppPlayer,
      turn,
      this);
  if (getIsFirst() || (smallBlindPosition == 1)) {
    callChild.reset(new OpponentNode(nodeParams));
  } else {
    callChild.reset(new ChoiceNode(nodeParams));
  }
  callChild->setIsAllIn(isAllInCheck(botPlayer, oppPlayer));
  callChild->setCurrentRaise(firstAction * currentRaise);
  callChild->getGame().getBotPlayer().setPotInvestment( firstAction * callChild->getGame().getBotPlayer().getPotInvestment());
  callChild->getGame().getOppPlayer().setPotInvestment( firstAction * callChild->getGame().getOppPlayer().getPotInvestment());
  callChild->setIsFirst(!firstAction);
}

void Node::raise(double raiseAmount) {
  // if raise all-in (or more) create AllInNode, handled by call
  Player botPlayer= game.getBotPlayer();
  Player oppPlayer= game.getOppPlayer();
  if (isAllInCheck(botPlayer, oppPlayer)) {
    // might be a bug here with runSelection repeatedly raising to all-in
    call();
    if (callChild->getGame().getPlayerTurn() == 0) {
      raiseChild.reset(new ChoiceNode(*static_cast<ChoiceNode*>(callChild.get())));
    } else {
      raiseChild.reset(new OpponentNode(*static_cast<OpponentNode*>(callChild.get())));
    }
    return;
  }

  setIsFirst(false);

  Player* currentP = game.getPlayerTurn() ? &oppPlayer : &botPlayer;
  double totalChips = currentP->getChips() + currentP->getPotInvestment();
  raiseAmount = std::min(totalChips, std::max(std::max(bigBlind, raiseAmount), 2*currentRaise));

  currentP->setChips(currentP->getChips() - (raiseAmount - currentP->getPotInvestment()));
  currentP->setPotInvestment(raiseAmount);
  raiseChild.reset(new OpponentNode(game.getState(),
        initialChips * 2 - currentP->getChips() - game.getOppPlayer().getChips(),
        game.getBoardCards(),
        botPlayer,
        oppPlayer,
        !(game.getPlayerTurn()),
        this));
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

void Node::advanceStage(Stage refStage, std::vector<int>& deck, int action) {
  *this = *(getChildNode(action));
  if (getGame().getState() != refStage) {
    setIsFirst(true);
    updateBoard(refStage, deck);
  }
  if (getGame().getState() == Stage::SHOWDOWN) {
    handleShowdown();
  }
  if (getIsAllIn()) {
    handleAllIn(deck);
  }
}

void Node::handleAllIn(std::vector<int>& deck) {
  for (Stage s = getGame().getState(); s < Stage::SHOWDOWN; ++s) {
    updateBoard(s, deck);
  }
  handleShowdown();
}
