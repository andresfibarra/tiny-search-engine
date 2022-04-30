# Makefile for 'querier' module
#
# Andres Ibarra, February 2022

L = ../libcs50
C = ../common

OBJS1 = querier.o queryScore.o
LIBS = 
LLIBS = $C/common.a $L/libcs50.a 

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$C -I$L
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

.PHONY = all test valgrind clean

all: queryScore.o querier 

querier: $(OBJS1)
	$(CC) $(CFLAGS) $(OBJS1) $(LLIBS) $(LIBS) -o querier

querier.o: querier.h

queryScore.o: queryScore.h

test:
	bash -v testing.sh 

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f querier
	rm -f core