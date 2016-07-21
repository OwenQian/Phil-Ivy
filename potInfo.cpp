#include "potInfo.h"

PotInfo::PotInfo() :
	potSize(0) { }

PotInfo::PotInfo(std::vector<Player> plist, double psize) :
	playerList(plist),
	potSize(psize) { }

