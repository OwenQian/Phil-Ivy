#include "GameObject.h"

GameObject::GameObject() :
  state(Stage::HOLECARDS),
  pot(0.0),
  playerTurn(0) { }

  GameObject::GameObject(
      Stage                 state,
      double              pot,
      std::vector<int>    boardCards,
      Player              botPlayer,
      Player              oppPlayer,
      int                 playerTurn) :
    state(state),
    pot(pot),
    boardCards(boardCards),
    botPlayer(botPlayer),
    oppPlayer(oppPlayer),
    playerTurn(playerTurn) { }
