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
		argv[1] = "Th";
		argv[2] = "9h";
	}
	int deck[52];
	init_deck(deck);
	int ourCards[2] = {cardToHex(argv[1]), cardToHex(argv[2])};
	int boardCards[] = {cardToHex("8s"), cardToHex("7s"), cardToHex("Ad")};


	std::cout << "ppot: " << (potentialPostFlop(ourCards, boardCards))[0] << std::endl;

	std::cout << "npot: " << (potentialPostFlop(ourCards, boardCards))[1] << std::endl;

}
