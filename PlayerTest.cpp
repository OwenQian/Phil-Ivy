#include "Player.h"

#include <iostream>
#include <vector>

int main() {
	Player Max(13, 15, 1000, 0);
	Player Owen;
	std::vector<int> v {1 , 2};
	Player Stephen(v, 1000, 0);
	std::cout << Max.getHoleCards()[0] << " " << Max.getHoleCards()[1] << std::endl;
	std::cout << Stephen.getHoleCards()[0] << " " << Stephen.getHoleCards()[1] << std::endl;

	return 0;
}
