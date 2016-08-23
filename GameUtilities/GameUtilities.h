#ifndef __GAME_UTILITIES_H
#define __GAME_UTILITIES_H

#include "Decision.h"
#include "Action.h"

#include <vector>
#include <memory>

class Player;
class ChoiceNode;
class OpponentNode;
class Node;

std::vector<int> deal(
						std::vector<int>	&previousDeck,
						int					state);
void printBoardCards(std::vector<int>);
//initializes deck array to appropriate hex values
void init_deck(std::vector<int>& deck);
void allocateChips (int whoWon, Node &currentNode);
void playGame(); 
std::vector<Player> playRound(Player botPlayer, Player oppPlayer); 
std::shared_ptr<OpponentNode> playTurn(std::shared_ptr<ChoiceNode>, std::vector<int> deck);
std::shared_ptr<ChoiceNode> playTurn(std::shared_ptr<OpponentNode>, std::vector<int> deck);

// MCTS helper functions
void conditionalDeal(Node& thisNode, int refStage, int compareStage, std::vector<int> deck, int stage);

#endif
