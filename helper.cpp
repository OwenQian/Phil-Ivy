#include <cctype>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

#include "helper.h"

int cardToHex(std::string s){
	const char rank = toupper(s[0]);
	const char suit = toupper(s[1]);
	int sum = 0;
	switch (rank) {
		case 'A':
			sum += (0x10000000 + 41 + 0xC00);
			break;
		case 'K':
			sum += (0x08000000 + 37 + 0xB00);
			break;
		case 'Q':
			sum += (0x04000000 + 31 + 0xA00);
			break;
		case 'J':
			sum += (0x02000000 + 29 + 0x900);
			break;
		case 'T':
			sum += (0x01000000 + 23 + 0x800);
			break;
		case '9':
			sum += (0x00800000 + 19 + 0x700);
			break;
		case '8':
			sum += (0x00400000 + 17 + 0x600);
			break;
		case '7':
			sum += (0x00200000 + 13 + 0x500);
			break;
		case '6':
			sum += (0x00100000 + 11 + 0x400);
			break;
		case '5':
			sum += (0x00080000 + 7 + 0x300);
			break;
		case '4':
			sum += (0x00040000 + 5 + 0x200);
			break;
		case '3':
			sum += (0x00020000 + 3 + 0x100);
			break;
		case '2':
			sum += (0x00010000 + 2 + 0x000);
			break;
		default:
			return -1;
			break;
	}

	switch (suit) {
		case 'S':
			sum += 0x00001000;
			break;
		case 'H':
			sum += 0x00002000;
			break;
		case 'D':
			sum += 0x00004000;
			break;
		case 'C':
			sum += 0x00008000;
			break;
		default:
			return -2;
			break;
	}
	return sum;
}

std::string hexToCard(int hex) {
	char first, second;
	const int rank = hex % (1 << 6);
	const int suit = (hex >> 12) % 16;
	switch (rank) {
		case 2:
			first = '2';
			break;
		case 3:
			first = '3';
			break;
		case 5:
			first = '4';
			break;
		case 7:
			first = '5';
			break;
		case 11:
			first = '6';
			break;
		case 13:
			first = '7';
			break;
		case 17:
			first = '8';
			break;
		case 19:
			first = '9';
			break;
		case 23:
			first = 'T';
			break;
		case 29:
			first = 'J';
			break;
		case 31:
			first = 'Q';
			break;
		case 37:
			first = 'K';
			break;
		case 41:
			first = 'A';
			break;
		default:
			return "error rank";
			break;
	}
	switch(suit) {
		case 1:
			second = 's';
			break;
		case 2:
			second = 'h';
			break;
		case 4:
			second = 'd';
			break;
		case 8:
			second = 'c';
			break;
		default:
			return "error suit";
			break;
	}
	char s[3];
	s[0] = first;
	s[1] = second;
	s[2] = '\0';
	return (s);
}

void init_deck(int deck[]) {
	int counter = 0;
	const char ranks[] = {'A', '2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K'};
	const char suits[] = {'s', 'h', 'd', 'c'};
	for (char i: ranks) {
		for (char j: suits) {
			char s[3];
			s[0] = i;
			s[1] = j;
			s[2] = '\0';
			deck[counter++] = cardToHex(s);
		}
	}
}

int hand_rank(int i) {
	if (i <= 10){
		return 1;
	} else if (i <= 166) {
		return 2;
	} else if (i <= 322) {
		return 3;
	} else if (i <= 1599) {
		return 4;
	} else if (i <= 1609) {
		return 5;
	} else if (i <= 2467){
		return 6;
	} else if (i <= 3325){
		return 7;
	} else if (i <= 6185){
		return 8;
	} else
		return 9;
}

//indexed by hexRank *2
void init_pairs(std::map<int, int>& pairs) {
	std::vector<std::string> ranks {"As", "Ks", "Qs", "Js", "Ts", "9s", "8s", "7s", "6s", "5s", "4s", "3s", "2s"};
	std::vector<int> intRanks;
	std::transform(ranks.begin(), ranks.end(), back_inserter(intRanks), cardToHex);
	for (std::vector<int>::iterator it = intRanks.begin();
			it != intRanks.end(); ++it) {
		*it = *it >> 16;
		*it = *it << 1;
	}

	int strength[13];
	for (int i = 12; i >= 0; --i) {
		strength[12 - i] = i + 1;
	}

	//add element to map
	for (int i = 0; i < 13; ++i) {
		std::pair<int, int> tempPair;
		pairs.insert(std::pair<int, int>(intRanks[i], strength[i]) );
	}
}

double preflop(int ourCards[2]) {
	//ahead-behind-tie
	int matchup[3] = {0, 0, 0};

	int oppCards[2];
	std::map<int, int> pairs;
	init_pairs(pairs);
	int ourPairCheck = (ourCards[0] >> 16) + (ourCards[1] >> 16);
	std::cout << "ourPairCheck: " << ourPairCheck << " our pair score: " << pairs[ourPairCheck] << std::endl;

	//checking if our hole cards contain a pair
	int deck[52];
	init_deck(deck);

	//assigning opponent cards
	for (int i = 0; i < 51; ++i) {
		if (deck[i] != ourCards[0] && deck[i] != ourCards[1])
			oppCards[0] = deck[i];
		else 
			continue;
		for (int j = i + 1; j < 52; ++j) {
			if (deck[j] != ourCards[0] && deck[j] != ourCards[1]) {
				oppCards[1] = deck[j];
				int oppPairCheck = (oppCards[0] >> 16) + (oppCards[1] >> 16);

				if (pairs[ourPairCheck]) {		//if we have a pair
					if (pairs[ourPairCheck] > pairs[oppPairCheck])
						matchup[0] += 1;
					else if (pairs[ourPairCheck] < pairs[oppPairCheck])
						matchup[1] += 1;
					else
						matchup[2] += 1;
				} else {		//we have a high card
					if (pairs[oppPairCheck] != 0) {	//if opp has pair, we lose
						matchup[1] += 1;
					} else {
						int ourHighCard = (ourCards[0] >> 16) + (ourCards[1] >> 16);
						int oppHighCard = (oppCards[0] >> 16) + (oppCards[1] >> 16);
						if (ourHighCard > oppHighCard)
							matchup[0] += 1;
						else if (ourHighCard < oppHighCard)
							matchup[1] += 1;
						else
							matchup[2] += 1;
					}
				}
			}
		}
	}
	double totalComparisons = (double) matchup[0] + (double) matchup[1] + (double) matchup[2];
	std::cout << "totalComparisons: " << totalComparisons;
	double aheadScore = (double) matchup[0] + ((double) matchup[2] / 2.0);
	std::cout << " aheadScore: " << aheadScore << std::endl;
	double returnScore = aheadScore / totalComparisons;
	return returnScore;
}
