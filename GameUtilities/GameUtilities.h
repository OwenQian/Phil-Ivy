#ifndef __GAME_UTILITIES_H
#define __GAME_UTILITIES_H

#include <vector>

std::vector<int> deal(	std::vector<int>	&previousDeck,
						int					state);
//initializes deck array to appropriate hex values
void init_deck(std::vector<int>& deck);


#endif