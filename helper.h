#ifndef __HELPER_H
#define __HELPER_H

#include <string>
#include <map>

//converts string card to hex equivalent
int cardToHex(std::string s = "Ah");
//converts hex to string equivalent
std::string hexToCard(int hex);
//initializes deck array to appropriate hex values
void init_deck(int deck[]);

//returns the corresponding hand, i.e., 1 for straight flush
int hand_rank(int i);

//evaluates
int currentStrength(int ourCards[], int stage, int boardCards[] = NULL);
//int[0] is positive potential, int[1] is negative potential
//number of possibilities that put us ahead, or behind
int potentialStrength();

double preflop(int ourCards[2]);
double flop(int outCards[], int boardCards[]);
double turn(int outCards[], int boardCards[]);
double river(int outCards[], int boardCards[]);

void init_pairs(std::map<int, int>& pairs);
#endif
