# Makefile for the 'common' library
#
# Andres Ibarra, February 2022

L = ../libcs50


OBJS = pagedir.o index.o word.o
LIB = common.a 
LIBS =  $L/libcs50.a 

CFLAGS = -Wall -pedantic -std=c11 -ggdb $(TESTING) -I$L
CC = gcc
MAKE = make
# for memory-leak tests
VALGRIND = valgrind --leak-check=full --show-leak-kinds=all

all: $(LIB)

$(LIB): $(OBJS)
	ar cr $(LIBS) $(LIB) $(OBJS)

pagedir.o: pagedir.h
word.o: word.h
index.o: index.h

.PHONY: all clean
	
clean:
	rm -rf *.dSYM  # MacOS debugger info
	rm -f *~ *.o
	rm -f pagedir
	rm -f core