#ifndef __GAME_UTILITIES_H
#define __GAME_UTILITIES_H

#include "Decision.h"
#include "../Action.h"
#include "../Stage.h"
#include "../GameObject.h"

#include <vector>
#include <memory>

class Player;
class ChoiceNode;
class OpponentNode;
class Node;

std::vector<int> deal(std::vector<int>&, Stage);
void printBoardCards(std::vector<int>);
void printCurrentStage(Stage);
void printChipSummaries(const GameObject& game);
void init_deck(std::vector<int>& deck);

// Node helper functions
void updateBoard(Node* node, Stage stage, std::vector<int>& deck);
void advanceStage(Node*&, Stage&, std::vector<int>&, int);
void collectBlinds(Node*);
void allocateChips (int whoWon, Node &currentNode);
void handleAllIn(Node*, std::vector<int>&);
void handleShowdown(Node*);

// MCTS helper functions
void conditionalDeal(Node& thisNode, Stage refStage, Stage compareStage, std::vector<int> &deck, Stage stage);

#endif
