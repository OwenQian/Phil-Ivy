#include <cctype>
#include <iostream>
#include <vector>
#include <algorithm>
#include <map>

#include "helper.h"

extern int comb6[6][5];
extern int comb7[21][5];
extern int eval_5hand_fast(int c1, int c2, int c3, int c4, int c5);

bool alreadyDealt(int dealtCards[], int beingDealt, int numOfCards /* = 2 */) {
	for (int i = 0; i < numOfCards; ++i) {
		if (beingDealt == dealtCards[i]) {
			return true;
		}
	}
	return false;
}

bool alreadyDealt(std::vector<int> dealtCards, int beingDealt) {
	for (int i: dealtCards) {
		if (beingDealt == i) {
			return true;
		}
	}
	return false;
}

int bestHand(int combNum, int cards[]) {
	int ret = 9999999;

	if (combNum == 6) {
		for (int i = 0; i < combNum; ++i) {
			ret = std::min( ret,
					eval_5hand_fast( cards[comb6[i][0]],
					cards[comb6[i][1]],
					cards[comb6[i][2]],
					cards[comb6[i][3]],
					cards[comb6[i][4]]) );
		}
	}
	else if (combNum == 21) {
		for (int i = 0; i < combNum; ++i) {
			ret = std::min( ret,
					eval_5hand_fast( cards[comb7[i][0]],
					cards[comb7[i][1]],
					cards[comb7[i][2]],
					cards[comb7[i][3]],
					cards[comb7[i][4]]) );
		}
	}
	return ret;
}

int cardToHex(std::string s) {
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
	const char ranks[] = {'2', '3', '4', '5', '6', '7', '8', '9', 'T', 'J', 'Q', 'K', 'A'};
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

double currentFlop(int ourCards[2], int boardCards[3]) {
	int deck[52];
	init_deck(deck);
	int matchup[3] = {0, 0, 0};
	int ourHandRank = 9999999;		//set to be higher than any hand-rank
	int oppHandRank = 9999999;

	//TODO clean-up this implementation of veccat
	//create a vector to hold ourCards + boardCards
	std::vector<int> allCards;
	for (int i = 0; i < 2; ++i)
		allCards.push_back(ourCards[i]);
	for (int i = 0; i < 3; ++i){
		allCards.push_back(boardCards[i]);
	}
	ourHandRank = eval_5hand_fast(allCards[0],
								allCards[1],
								allCards[2],
								allCards[3],
								allCards[4]);

	//iterate through all the cards in deck
	for (int i = 0; i < 51; ++i) {
		if (deck[i] != ourCards[0] &&
				deck[i] != ourCards[1] &&
				deck[i] != boardCards[0] &&
				deck[i] != boardCards[1] &&
				deck[i] != boardCards[2])
		{
			allCards[0] = deck[i];
		}
		else 
			continue;

		for (int j = i + 1; j < 52; ++j) {
			if (deck[j] != ourCards[0] && 
					deck[j] != ourCards[1] &&
					deck[j] != boardCards[0] &&
					deck[j] != boardCards[1] &&
					deck[j] != boardCards[2])
			{
				allCards[1] = deck[j];
				oppHandRank = eval_5hand_fast(allCards[0],
								allCards[1],
								allCards[2],
								allCards[3],
								allCards[4]);

				if (oppHandRank > ourHandRank){	//lower is better
					matchup[0] += 1;
				} else if (oppHandRank < ourHandRank){
					matchup[1] += 1;
				} else {
					matchup[2] += 1;
				}
			}
		}
	}
	double totalComparisons = (double) matchup[0] + (double) matchup[1] + (double) matchup[2];
	//std::cout << "totalComparisons: " << totalComparisons << std::endl;
	double aheadScore = (double) matchup[0] + ((double) matchup[2] / 2.0);
	//std::cout << "aheadScore: " << aheadScore << std::endl;
	//std::cout << "tieScore: " << matchup[2] << std::endl;
	double returnScore = aheadScore / totalComparisons;
	return returnScore;
}

double currentTurn(int ourCards[2], int boardCards[4]) {
	int deck[52];
	init_deck(deck);
	int matchup[3] = {0, 0, 0};
	int ourHandRank = 9999999;		//set to be higher than any hand-rank
	int oppHandRank = 9999999;

	//TODO clean-up this implementation of veccat
	//create a vector to hold ourCards + boardCards
	std::vector<int> allCards;
	for (int i = 0; i < 2; ++i)
		allCards.push_back(ourCards[i]);
	for (int i = 0; i < 4; ++i){
		allCards.push_back(boardCards[i]);
	}

	//6 = 6C5
	for (int i = 0; i < 6; ++i) {
		//find the strongest 5 card combination (lowest number)
		ourHandRank = std::min( ourHandRank,
				eval_5hand_fast(allCards[comb6[i][0]],
				allCards[comb6[i][1]],
				allCards[comb6[i][2]],
				allCards[comb6[i][3]],
				allCards[comb6[i][4]]) );
	}
	//iterate through all the cards in deck
	for (int i = 0; i < 51; ++i) {
		if (deck[i] != ourCards[0] &&
				deck[i] != ourCards[1] &&
				deck[i] != boardCards[0] &&
				deck[i] != boardCards[1] &&
				deck[i] != boardCards[2] &&
				deck[i] != boardCards[3])
		{
			allCards[0] = deck[i];
		}
		else 
			continue;

		for (int j = i + 1; j < 52; ++j) {
			if (deck[j] != ourCards[0] && 
					deck[j] != ourCards[1] &&
					deck[j] != boardCards[0] &&
					deck[j] != boardCards[1] &&
					deck[j] != boardCards[2] &&
					deck[j] != boardCards[3])
			{
				allCards[1] = deck[j];
				oppHandRank = 9999999;

				//6 = 6C5
				for (int k = 0; k < 6; ++k) {
					//find the strongest 5 card combination (lowest number)
					oppHandRank = std::min( oppHandRank,
							eval_5hand_fast(allCards[comb6[k][0]],
							allCards[comb6[k][1]],
							allCards[comb6[k][2]],
							allCards[comb6[k][3]],
							allCards[comb6[k][4]]) );
				}
				if (oppHandRank > ourHandRank){	//lower is better
					matchup[0] += 1;
				} else if (oppHandRank < ourHandRank){
					matchup[1] += 1;
				} else {
					matchup[2] += 1;
				}
			}
		}
	}
	double totalComparisons = (double) matchup[0] + (double) matchup[1] + (double) matchup[2];
	double aheadScore = (double) matchup[0] + ((double) matchup[2] / 2.0);
	double returnScore = aheadScore / totalComparisons;
	return returnScore;
}

double currentRiver(int ourCards[2], int boardCards[]) {
	int deck[52];
	init_deck(deck);
	int matchup[3] = {0, 0, 0};
	int ourHandRank = 9999999;		//set to be higher than any hand-rank
	int oppHandRank = 9999999;

	//TODO clean-up this implementation of veccat
	//create a vector to hold ourCards + boardCards
	std::vector<int> allCards;
	for (int i = 0; i < 2; ++i)
		allCards.push_back(ourCards[i]);
	for (int i = 0; i < 5; ++i){
		allCards.push_back(boardCards[i]);
	}

	//21 = 7C5
	for (int i = 0; i < 21; ++i) {
		ourHandRank = std::min( ourHandRank,
				eval_5hand_fast(allCards[comb7[i][0]],
				allCards[comb7[i][1]],
				allCards[comb7[i][2]],
				allCards[comb7[i][3]],
				allCards[comb7[i][4]]) );
	}
	
	//iterate through all untaken cards in deck
	for (int i = 0; i < 51; ++i) {
		if (deck[i] != ourCards[0] &&
				deck[i] != ourCards[1] &&
				deck[i] != boardCards[0] &&
				deck[i] != boardCards[1] &&
				deck[i] != boardCards[2] &&
				deck[i] != boardCards[3] &&
				deck[i] != boardCards[4])
		{
				allCards[0] = deck[i];
		} else
			continue;

		for (int j = i + 1; j < 52; ++j) {
			if (deck[j] != ourCards[0] &&
					deck[j] != ourCards[1] &&
					deck[j] != boardCards[0] &&
					deck[j] != boardCards[1] &&
					deck[j] != boardCards[2] &&
					deck[j] != boardCards[3] &&
					deck[j] != boardCards[4])
			{
				allCards[1] = deck[j];
				oppHandRank = 9999999;

				//21 = 7C5
				for (int k = 0; k < 21; ++k) {
					oppHandRank = std::min( oppHandRank,
							eval_5hand_fast(allCards[comb7[k][0]],
								allCards[comb7[k][1]],
								allCards[comb7[k][2]],
								allCards[comb7[k][3]],
								allCards[comb7[k][4]]) );
				}
				if (oppHandRank > ourHandRank){	//lower is better
					matchup[0] += 1;
				} else if (oppHandRank < ourHandRank){
					matchup[1] += 1;
				} else {
					matchup[2] += 1;
				}
			}
		}
	}
	double totalComparisons = (double) matchup[0] + (double) matchup[1] + (double) matchup[2];
	//std::cout << "totalComparisons: " << totalComparisons << std::endl;
	double aheadScore = (double) matchup[0] + ((double) matchup[2] / 2.0);
	//std::cout << "aheadScore: " << aheadScore << std::endl;
	//std::cout << "tieScore: " << matchup[2] << std::endl;
	double returnScore = aheadScore / totalComparisons;
	return returnScore;
}

double currentPreflop(int ourCards[2]) {
	//ahead-behind-tie
	int matchup[3] = {0, 0, 0};

	int oppCards[2];
	std::map<int, int> pairs;
	init_pairs(pairs);
	int ourPairCheck = (ourCards[0] >> 16) + (ourCards[1] >> 16);

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


std::vector<double> potentialPostFlop(int ourCards[], int boardCards[]){
	int matchup[3] = {0, 0, 0};
	int matchupPot[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
	int index;
	int oppCards[2];

	//checking if our hole cards contain a pair
	int deck[52];
	init_deck(deck);
	
	for (int i = 0; i < 51; ++i){
		if (deck[i] != ourCards[0] && deck[i] != ourCards[1])
			oppCards[0] = deck[i];
		else 
			continue;
		for (int j = i + 1; j < 52; ++j){
			if (deck[j] != ourCards[0] && deck[j] != ourCards[1]) {
				std::cout << "asdfasdf" << std::endl;
				oppCards[1] = deck[j];
				std::vector<int> ourHand;
				ourHand.reserve(7);
				ourHand.insert(ourHand.end(), ourCards, ourCards + 2 );
				ourHand.insert(ourHand.end(), boardCards, boardCards + 3 );
			
				std::vector<int> oppHand;
				oppHand.reserve(7);
				oppHand.insert(oppHand.end(), oppCards, oppCards + 2 );
				oppHand.insert(oppHand.end(), boardCards, boardCards + 3 );
				
				if (eval_5hand_fast(ourHand[1], ourHand[2],ourHand[3],ourHand[4],ourHand[0]) 
					< eval_5hand_fast(oppHand[1], oppHand[2], oppHand[3], oppHand[4], oppHand[0])){
					index = 0;
				} else if (eval_5hand_fast(ourHand[1], ourHand[2],ourHand[3],ourHand[4],ourHand[0]) 
					> eval_5hand_fast(oppHand[1], oppHand[2], oppHand[3], oppHand[4], oppHand[0])){
					index = 1;
				} else {
					index = 2;
				}
				matchup[index] += 1;
				std::cout << "asdfasdf" << std::endl;
				for (int k1 = 0; k1 < 51; ++k1){
					if ( !alreadyDealt(ourHand, deck[k1]) && !alreadyDealt(oppHand, deck[k1]) ) {
						ourHand.push_back(deck[k1]);
						oppHand.push_back(deck[k1]);
						for (int k2 = k1 + 1; k2 < 52; ++k2) {
							if ( !alreadyDealt(ourHand, deck[k2]) && !alreadyDealt(oppHand, deck[k2]) ) {
								ourHand.push_back(deck[k2]);
								for (int a = 0; a < 7; ++a){
					std::cout << "card" << a << ": " << hexToCard(ourHand[a]) << std::endl;
				}
								oppHand.push_back(deck[k2]);
								for (int a = 0; a < 7; ++a){
					std::cout << "card" << a << ": " << hexToCard(oppHand[a]) << std::endl;
				}
								int ourhandrank = bestHand(21, ourHand.data());
							
								int opphandrank = bestHand(21, oppHand.data());
								if (ourhandrank < opphandrank){
									matchupPot[index][0] += 1;
								} else if (ourhandrank > opphandrank){
									matchupPot[index][1] += 1;
								} else {
									matchupPot[index][2] += 1;
								}
							}
					}
				}
			}
		}
	}
	}//indent
	// behind-ahead + (tie-ahead + behind-tie) / 2
	double pPotential = (double) matchupPot[1][0] + (( (double)matchupPot[2][0] + (double) matchupPot[1][2]) / 2.0);
	// pPotential / behind + tie
	pPotential = pPotential / (double) (matchup[1] + matchup[2]);

	// ahead-behind + (tie-behind + ahead-tie) / 2
	double nPotential = (double) matchupPot[0][1] + (( (double)matchupPot[2][1] + matchupPot[0][2]) / 2.0);
	// nPotential / ahead + tie
	nPotential = nPotential / (double) (matchup[0] + matchup[2]);
	std::vector<double> ret(pPotential, nPotential);
	return ret;
}

std::vector<double> potentialPreFlop(int ourCards[]) {
	int matchup[3] = {0, 0, 0};
	int matchupPot[3][3] = {{0, 0, 0}, {0, 0, 0}, {0, 0, 0}};
	int index;
	int oppCards[2];
	std::map<int, int> pairs;
	init_pairs(pairs);
	int ourPairCheck = (ourCards[0] >> 16) + (ourCards[1] >> 16);
	int ourHandRank = 9999999;		//set to be higher than any hand-rank
	int oppHandRank = 9999999;

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
						index = 0;
					else if (pairs[ourPairCheck] < pairs[oppPairCheck])
						index = 1;
					else
						index = 2;
				} else {		//we have a high card
					if (pairs[oppPairCheck] != 0) {	//if opp has pair, we lose
						index = 1;
					} else {
						int ourHighCard = (ourCards[0] >> 16) + (ourCards[1] >> 16);
						int oppHighCard = (oppCards[0] >> 16) + (oppCards[1] >> 16);
						if (ourHighCard > oppHighCard)
							index = 0;
						else if (ourHighCard < oppHighCard)
							index = 1;
						else
							index = 2;
					}
				}
				matchup[index] += 1;

				std::vector<int> potentialBoardCards;
				for (int k1 = 0; k1< 47; ++k1) {
					if ( !alreadyDealt(ourCards, deck[k1]) && !alreadyDealt(oppCards, deck[k1]) ) {
						potentialBoardCards.push_back( deck[k1]);

						for (int k2 = k1 + 1; k2 < 48; ++k2) {
							if ( !alreadyDealt(ourCards, deck[k2]) && !alreadyDealt(oppCards, deck[k2]) ) {
								potentialBoardCards.push_back( deck[k2] );

								for (int k3 = k2 + 1; k3 < 49; ++k3) {
									if ( !alreadyDealt(ourCards, deck[k3]) && !alreadyDealt(oppCards, deck[k3]) ) {
										potentialBoardCards.push_back( deck[k3] );

										for (int k4 = k3 + 1; k4 < 50; ++k4) {
											if ( !alreadyDealt(ourCards, deck[k4]) && !alreadyDealt(oppCards, deck[k4]) ) {
												potentialBoardCards.push_back(deck[k4]);

												for (int k5 = k4 + 1; k5 < 51; ++k5) {
													if ( !alreadyDealt(ourCards, deck[k5]) && !alreadyDealt(oppCards, deck[k5]) ) {
														potentialBoardCards.push_back(deck[k5]);
														//all the potential board cards have been dealt, compare vs. our hand
														std::vector<int> ourAllCards = potentialBoardCards;
														std::vector<int> oppAllCards = potentialBoardCards;
														ourAllCards.push_back(ourCards[0]);
														ourAllCards.push_back(ourCards[1]);
														oppAllCards.push_back(oppCards[0]);
														oppAllCards.push_back(oppCards[1]);
														ourHandRank = bestHand(21, ourAllCards.data());
														oppHandRank = bestHand(21, oppAllCards.data());

														if (ourHandRank < oppHandRank) {
															matchupPot[index][0] += 1;
														} else if (ourHandRank > oppHandRank) {
															matchupPot[index][1] += 1;
														} else {
															matchupPot[index][2] += 1;
														}
													} else
														continue;
												}
											} else
												continue;
										}
									} else
										continue;
								}
							} else
								continue;
						}
					} else
						continue;
				}
			}
		}
	}
	// behind-ahead + (tie-ahead + behind-tie) / 2
	double pPotential = (double) matchupPot[1][0] + (( (double)matchupPot[2][0] + (double) matchupPot[1][2]) / 2.0);
	// pPotential / behind + tie
	pPotential = pPotential / (double) (matchup[1] + matchup[2]);

	// ahead-behind + (tie-behind + ahead-tie) / 2
	double nPotential = (double) matchupPot[0][1] + (( (double)matchupPot[2][1] + matchupPot[0][2]) / 2.0);
	// nPotential / ahead + tie
	nPotential = nPotential / (double) (matchup[0] + matchup[2]);
	std::vector<double> ret(pPotential, nPotential);
	return ret;
}
