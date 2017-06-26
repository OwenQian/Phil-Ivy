#ifndef GAME_INFO_H
#define GAME_INFO_H

#include "Player.h"
#include "Stage.h"

#include <vector>

class GameObject {
  private:
    Stage state;
    double pot;
    std::vector<int> boardCards;
    Player botPlayer;
    Player oppPlayer;
    int playerTurn;

  public:
    // Constructors
    GameObject();
    GameObject(Stage            state,
               double           pot,
               std::vector<int> boardCards,
               Player           botPlayer,
               Player           oppPlayer,
               int              playerTurn);

    // Member-accessibility functions
    // Getters
    Stage getState() const { return state; }
    std::vector<int>& getBoardCards() { return boardCards; }
    std::vector<int> const & getBoardCards() const { return boardCards; }
    Player& getBotPlayer() { return botPlayer; }
    Player const & getBotPlayer() const { return botPlayer; }
    Player& getOppPlayer() { return oppPlayer; }
    Player const & getOppPlayer() const { return oppPlayer; }
    double getPot() const { return pot; }
    int getPlayerTurn() const { return playerTurn; }

    // Setter; Note: be careful about changing boardCards
    void setBoardCards(std::vector<int> boardCards) { this->boardCards = boardCards; }
    void setPlayerTurn(int turn) { this->playerTurn = turn; }
    void setBotPlayer(const Player& rhs) { botPlayer = rhs; }
    void setOppPlayer(const Player& rhs) { oppPlayer = rhs; }
};

#endif //GameObject.h
