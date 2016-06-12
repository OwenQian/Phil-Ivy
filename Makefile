CC = g++
CFLAGS = -Wall -std=c++11 -g
OBJECTS = helper.o arrays.o fast_eval.o allfive.o

allfive: allfive.o
	$(CC) $(CFLAGS) $(OBJECTS) -o allfive

allfive.o: helper.o arrays.o fast_eval.o allfive.cpp poker.h
	$(CC) $(CFLAGS) -c allfive.cpp

fast_eval.o: fast_eval.cpp
	$(CC) $(CFLAGS) -c $<

arrays.o: arrays.cpp
	$(CC) $(CFLAGS) -c $<

helper.o: helper.cpp helper.h
	$(CC) $(CFLAGS) -c $<

main: main.cpp helper.cpp helper.h
	$(CC) $(CFLAGS) fast_eval.cpp main.cpp helper.cpp arrays.cpp -o main
