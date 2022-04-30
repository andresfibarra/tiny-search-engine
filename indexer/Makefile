# Makefile for 'indexer' module
#
# Andres Ibarra, February 2022

L = ../libcs50
C = ../common

OBJS1 = indexer.o
OBJS2 = indexTest.o
LIBS = 
LLIBS = $C/common.a $L/libcs50.a 

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$C -I$L
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

.PHONY = all test valgrind clean

all: indexer indexTest

indexer: $(OBJS1)
	$(CC) $(CFLAGS) $(OBJS1) $(LLIBS) $(LIBS) -o indexer

indexTest: $(OBJS2)
	$(CC) $(CFLAGS) $(OBJS2) $(LLIBS) $(LIBS) -o indexTest

indexer.o: indexer.h

indexTest.o: 

test:
	bash -v testing.sh &> testing.out

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f indexer
	rm -f core