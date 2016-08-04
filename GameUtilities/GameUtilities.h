#ifndef __GAME_UTILITIES_H
#define __GAME_UTILITIES_H

#include <vector>
#include <memory>

class Player;
class Node;

std::vector<int> deal(
						std::vector<int>	&previousDeck,
						int					state);
//initializes deck array to appropriate hex values
void init_deck(std::vector<int>& deck);

void playGame(); 
std::vector<Player> playRound(Player botPlayer, Player oppPlayer); 
std::shared_ptr<Node> playTurn(std::shared_ptr<Node> currentNode);

#endif
