June 6, 2016-- 10:32 PM
##PLAN
- By Sunday, June 12, 2016
	- Working hand evaluator

# Missing Files & Functions
- pokerlib.c
- eval_5hand
	- supposed supplanted by eval_5hand_fast
- flushes
	- array which holds value of specific flush, should be in array.h
- hand_rank
- init_deck
- unique5
	- array which holds value of specific straight, should be in array.h

# Notes
- flushes[q]
	- each suit given unique power of 2, all anded together => if one isn't in same suite then we get 0 == false
- cards composed of adding the suit to the rank
- straights of ace to 5 should have an exception coded somewhere (edgecase)
