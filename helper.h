#ifndef __HELPER_H
#define __HELPER_H

#include <string>

//converts string card to hex equivalent
int cardToHex(std::string s = "Ah");
//converts hex to string equivalent
std::string hexToCard(int hex);
//initializes deck array to appropriate hex values
void init_deck(int deck[]);

#endif
