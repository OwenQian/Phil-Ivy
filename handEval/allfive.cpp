//#include <stdio.h>
#include <iostream>
#include <iomanip>
#include <cstdlib>
#include "poker.h"
#include "helper.h"

/*************************************************/
/*                                               */
/* This code tests my evaluator, by looping over */
/* all 2,598,960 possible five card hands, cal-  */
/* culating each hand's distinct value, and dis- */
/* playing the frequency count of each hand type */
/*                                               */
/* Kevin L. Suffecool, 2001                      */
/* suffecool@bigfoot.com                         */
/*                                               */
/*************************************************/

extern int eval_5hand_fast(int c1, int c2, int c3, int c4, int c5);
int main()
{
	int deck[52] = {0};
	int hand[5], freq[10];
	int a, b, c, d, e, i, j;

	// seed the random number generator
	//srand48( getpid() );

	// initialize the deck
	init_deck( deck );

	// zero out the frequency array
	for ( i = 0; i < 10; i++ )
		freq[i] = 0;

	// loop over every possible five-card hand
	for(a=0;a<48;a++)
	{
		hand[0] = deck[a];
		for(b=a+1;b<49;b++)
		{
			hand[1] = deck[b];
			for(c=b+1;c<50;c++)
			{
				hand[2] = deck[c];
				for(d=c+1;d<51;d++)
				{
					hand[3] = deck[d];
					for(e=d+1;e<52;e++)
					{
						hand[4] = deck[e];

						i = eval_5hand_fast( hand[0], hand[1], hand[2], hand[3], hand[4] );
						j = hand_rank(i);
						freq[j]++;
					}
				}
			}
		}
	}

	for(i=1;i<=9;i++) {
		const double totalComb = 2598960.0;
		double percent = double (freq[i]) / totalComb;
		percent *= 100.0;
		std::cout << std::setw(20) << value_str[i] << ": " << std::setw(10) << freq[i] << "; " <<
			std::setprecision(4) << percent << "\n";
		//printf( "%15s: %8d\n", value_str[i], freq[i] );
	}
}
