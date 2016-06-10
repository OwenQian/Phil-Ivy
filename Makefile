CC = g++
CFLAGS = -Wall -std=c++11 -g
OBJECTS = helper.o arrays.o fast_eval.o allfive.o

allfive: allfive.o
	$(CC) $(CFLAGS) $(OBJECTS) -o allfive

allfive.o: helper.o arrays.o fast_eval.o allfive.c poker.h
	$(CC) $(CFLAGS) -c allfive.c

fast_eval.o: fast_eval.c
	$(CC) $(CFLAGS) -c $<

arrays.o: arrays.cpp
	$(CC) $(CFLAGS) -c $<

helper.o: helper.cpp helper.h
	$(CC) $(CFLAGS) -c $<

main: main.cpp helper.cpp helper.h
	g++ -std=c++11 main.cpp helper.cpp arrays.cpp -o main
