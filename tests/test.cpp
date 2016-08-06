#include "helper.h"

#include <vector>
#include <iostream>
#include <chrono>

int main() {
	std::vector<int> deck, dealtCards;
	init_deck(deck);
	std::chrono::high_resolution_clock::time_point t1 = std::chrono::high_resolution_clock::now();
	for (int n = 0; n < 50; ++n) {
		dealtCards = deal(deck, PREFLOP); 
		std::cout << "######## New Deal #######" << std::endl;
		for (auto i = dealtCards.begin(); i != dealtCards.end(); ++i) {
			std::cout << hexToCard(*i) << std::endl;
		}
	}
	std::chrono::high_resolution_clock::time_point t2 = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count();
	std::cout << duration << std::endl;

	return 0;
}
