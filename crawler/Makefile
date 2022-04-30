# Makefile for 'crawler' module
#
# Andres Ibarra, February 2022

L = ../libcs50
C = ../common

OBJS = crawler.o
LIBS = 
LLIBS =  $L/libcs50.a $C/common.a

# uncomment the following to turn on verbose memory logging
# TESTING=-DMEMTEST

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$C -I$L
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

.PHONY: all test valgrind clean

all: crawler

crawler: $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LLIBS) $(LIBS) -o crawler
	

crawler.o: $L/webpage.h $C/pagedir.h $L/bag.h $L/hashtable.h

test:
	bash -v testing.sh

clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f crawler
	rm -f core