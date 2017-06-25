#include <memory>
#include <iostream>
#include <cassert>

#include "ChoiceNode.h"
#include "OpponentNode.h"
#include "../Config.h"
#include "../GameUtilities/Decision.h"
#include "../GameUtilities/GameUtilities.h"

ChoiceNode::ChoiceNode() :
  Node::Node() { }

  ChoiceNode::ChoiceNode(int state,
      double pot,
      std::vector<int> boardCards,
      Player botPlayer,
      Player oppPlayer,
      int playerTurn,
      Node* const parent) :
    Node::Node(state,
        pot,
        boardCards,
        botPlayer,
        oppPlayer,
        playerTurn,
        parent) { }

    ChoiceNode::ChoiceNode(const ChoiceNode& obj) :
      Node(obj) { }

      ChoiceNode::ChoiceNode(const Node& obj) :
        Node(obj) { }

        void ChoiceNode::fold() {
          foldChild.reset(new ChoiceNode(*this));
          //foldChild->getGame().setPlayerTurn(!foldChild->getGame().getPlayerTurn());
          foldChild->setParent(this);
          foldChild->setIsFolded(true);
          foldChild->setVisitCount(0);
        }

void ChoiceNode::call() {
  Player botPlayer = game.getBotPlayer();
  Player oppPlayer = game.getOppPlayer();
  botPlayer.setChips(botPlayer.getChips() - (currentRaise - botPlayer.getPotInvestment()));
  botPlayer.setPotInvestment(currentRaise);
  int turn = !game.getPlayerTurn();
  if (!getIsFirst()) {
    turn = smallBlindPosition;
  }
  NodeParamObject nodeParams(game.getState() + !getIsFirst(),
      initialChips * 2 - botPlayer.getChips() - game.getOppPlayer().getChips(),
      game.getBoardCards(),
      botPlayer,
      oppPlayer,
      turn,
      this);
  if (getIsFirst() || (smallBlindPosition == 1)) {
    callChild.reset(new OpponentNode(nodeParams));
  } else {
    callChild.reset( new ChoiceNode(nodeParams) );
  }
  callChild->setIsAllIn(isAllInCheck(botPlayer, oppPlayer));
  callChild->setCurrentRaise(firstAction * currentRaise);
  callChild->getGame().getBotPlayer().setPotInvestment( firstAction * callChild->getGame().getBotPlayer().getPotInvestment());
  callChild->getGame().getOppPlayer().setPotInvestment( firstAction * callChild->getGame().getOppPlayer().getPotInvestment());
  callChild->setIsFirst(!firstAction);
}

void ChoiceNode::raise(double raiseAmount) {
  // set to min raise
  if (raiseAmount < bigBlind || raiseAmount < 2*currentRaise) {
    raiseAmount = bigBlind > (2*currentRaise) ? bigBlind : (2*currentRaise);
  } 

  // if raise all-in (or more) create AllInNode, handled by call
  if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
      game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
    // might be a bug here with runSelection repeatedly raising to all-in
    call();
    if (callChild->getGame().getPlayerTurn() == 0) {
      raiseChild.reset(new ChoiceNode(*static_cast<ChoiceNode*>(callChild.get())));
    } else {
      raiseChild.reset(new OpponentNode(*static_cast<OpponentNode*>(callChild.get())));
    }
    return;
  }

  if (raiseAmount >= game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() ||
      raiseAmount >= game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() ) {

    // set raiseAmount to lesser of chip amounts
    raiseAmount = std::min(game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment(),
        game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment());
  }
  setIsFirst(false);
  Player tempPlayer = game.getBotPlayer();
  tempPlayer.setChips(tempPlayer.getChips() - (raiseAmount - tempPlayer.getPotInvestment()) );
  tempPlayer.setPotInvestment(raiseAmount);
  raiseChild.reset(new OpponentNode( game.getState(),
        initialChips * 2 - tempPlayer.getChips() - game.getOppPlayer().getChips(),
        game.getBoardCards(),
        tempPlayer,
        game.getOppPlayer(),
        !(game.getPlayerTurn()),
        this ));
  raiseChild->setCurrentRaise(raiseAmount);
}

Decision ChoiceNode::makeDecision(std::vector<int> deck) {
  Decision decision;

  //int temp;
  //std::cin >> temp;
  //decision.action = static_cast<Action>(temp);

  decision.action = monteCarlo(monteCarloDuration, deck);
  std::cout << "Bot Decision: " << static_cast<int>(decision.action) << std::endl;
  if (decision.action == Action::RAISE) {
    if (game.getBotPlayer().getChips() + game.getBotPlayer().getPotInvestment() <= currentRaise ||
        game.getOppPlayer().getChips() + game.getOppPlayer().getPotInvestment() <= currentRaise) {
      decision.raiseAmount = 1;
    } else {
      std::cout << "Enter Raise amount" << std::endl;
      double amount;
      std::cin >> amount;
      decision.raiseAmount = amount;
    }
  }
  return decision;
}
