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
	int boardCards[] = {cardToHex("Ah"), cardToHex("Ac"), cardToHex("Ad"), cardToHex("2c"), cardToHex("Th")};
	turn(ourCards, boardCards);

	std::cout << flop(ourCards, boardCards) << std::endl;
}
