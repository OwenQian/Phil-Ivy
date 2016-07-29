#ifndef __HELPER_H
#define __HELPER_H

#include <string>
#include <map>
#include <vector>

std::vector<int> deal(	std::vector<int>	previousDeck,
						int					state);

//converts string card to hex equivalent
int cardToHex(std::string s = "Ah");
//converts hex to string equivalent
std::string hexToCard(int hex);
//initializes deck array to appropriate hex values
void init_deck(std::vector<int>& deck);

//returns the corresponding hand, i.e., 1 for straight flush
int hand_rank(int i);

//evaluates
double currentStrength(int ourCards[], int stage, int boardCards[] = NULL);
//int[0] is positive potential, int[1] is negative potential
//number of possibilities that put us ahead, or behind

std::vector<double> potentialPreFlop(int ourCards[]);
std::vector<double> potentialPostFlop(int ourCards[], int boardCards[]);

double currentPreflop(int ourCards[2]);
double currentFlop(int outCards[], int boardCards[]);
double currentTurn(int outCards[], int boardCards[]);
double currentRiver(int outCards[], int boardCards[]);

//used to help deal cards
bool alreadyDealt(int dealtCards[], int beingDealt, int numOfCards = 2);
bool alreadyDealt(std::vector<int> dealtCards, int beingDealt);

//returns the score of the best hand out of perm6 or perm7
int bestHand(int combNum, int cards[]);

//std::vector<double> potentialStrength(int ourCards[], int stage, int boardCards[]);
//std::vector<double> potentialPreFlop(int ourCards[]);

void init_pairs(std::map<int, int>& pairs);
#endif
