#ifndef PLAYER_H
#define PLAYER_H

#include <vector>

class Player {
  private:
    std::vector<int> holeCards;
    double chips;
    double potInvestment;

  public:

    Player();
    // Creates players with appropriate values
    Player(std::vector<int> hcards, double chip, double pi);
    Player(int c1, int c2, double chip, double pi);

    // Member accessibility fcns

    // Getters
    std::vector<int> getHoleCards() const { return holeCards; }
    double getChips() const { return chips; }
    double getChips() { return chips; }
    double getPotInvestment() const { return potInvestment; }

    // Setters
    void setHoleCards(std::vector<int> hCards) { holeCards = hCards; }
    void setHoleCards(int c1, int c2);
    void setChips(double c) { chips = c; }
    void addChips(double c) { chips += c; }
    void setPotInvestment(double pi) { potInvestment = pi; }

    Player& operator=(const Player& rhs);
};

inline void Player::setHoleCards(int c1, int c2) {
  std::vector<int> v{c1, c2};
  holeCards = v;
}

#endif
