#include <iostream>
#include <cctype>

#include "helper.h"
//#include <stdio.h>

int main() {
	int deck[52] = {0};
	init_deck(deck);
	for (int i = 0; i < 52; ++i) {
		std::cout << hexToCard(deck[i]) << std::endl;
	}
}

