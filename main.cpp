#include <iostream>
//#include <stdio.h>

int main() {
	int c1 = (0x1000);	//ace of spades
	int c2 = (0x1000);	//king of diamonds
	int c3 = (0x1000);	//3 of hearts
	int c4 = (0x1000);	//7 of hearts
	int c5 = (0x1001);	//10 of spades

    int q = (c1 | c2 | c3 | c4 | c5) >> 16;
	std::cout << q << std::endl;

	return 0;
}
