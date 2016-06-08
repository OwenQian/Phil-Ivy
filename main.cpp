#include <iostream>
//#include <stdio.h>

int main() {
	int c1 = (0x00010000);	//ace of spades
	int c2 = (0x10000000);	//king of diamonds
	int c3 = (0x10000000);	//3 of hearts
	int c4 = (0x10000000);	//7 of hearts
	int c5 = (0x10010000);	//10 of spades

    int q = (c1 | c2 | c3 | c4 | c5) >> 16;
	std::cout << q << std::endl;

	return 0;
}
