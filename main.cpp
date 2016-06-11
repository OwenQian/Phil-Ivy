#include <iostream>
#include <cctype>
#include <map>
#include <vector>
#include <algorithm>

#include "helper.h"
//#include <stdio.h>

int main() {
	std::map<int, int> pairs;
	init_pairs(pairs);

	std::cout << "size: " << pairs.size() << std::endl;

	for (std::map<int, int>::const_iterator it = pairs.begin(); it != pairs.end(); ++it) {
		std::cout << it->second << std::endl;
	}
	return 0;
}

