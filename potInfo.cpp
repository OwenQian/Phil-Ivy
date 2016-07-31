#include "potInfo.h"

PotInfo::PotInfo() :
	potSize(0) { }

PotInfo::PotInfo(Player bPlayer, Player oPlayer, double psize) :
	botPlayer(bPlayer),
	oppPlayer(oPlayer),
	potSize(psize) { }
