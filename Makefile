CXX      = g++
CXXFLAGS = -Wall -std=c++11

SRCS     = Player.cpp GameObject.cpp PlayerTest.cpp potTest.cpp potInfo.cpp GameTest.cpp

PlayerTest: PlayerTest.cpp Player.cpp Player.h
	$(CXX) $(CXXFLAGS) $^ -o $@
