#include <iostream>
#include <cctype>
#include <map>
#include <vector>
#include <algorithm>
#include <string>

#include "helper.h"
//#include <stdio.h>

int main(int argc, char* argv[]) {
	if (argc < 2) {
		argv[1] = "7s";
		argv[2] = "2d";
	}
	int deck[52];
	init_deck(deck);
	int ourCards[2] = {cardToHex(argv[1]), cardToHex(argv[2])};
	int boardCards[] = {cardToHex("Tc"), cardToHex("Ad"), cardToHex("Ac"), cardToHex("Ah")};
	turn(ourCards, boardCards);

	//std::cout << preflop(mainourCards) << std::endl;
}
