#!/bin/bash
# Testing script for indexer.h
# Run in make test as bash -v testing.sh &> testing.out
# Andres Ibarra, Winter 2022

########### INVALID ARGUMENTS ############
# No arguments
./indexer

# One argument 
./indexer ../data/letters10

# Three or more arguments 
./indexer ../data/letters10 ../data/indexLetters10 extraArg

# invalid pageDirectory (non-existent path) 
./indexer ../data/fakeDir ../data/indexLetters10

# invalid pageDirectory (not a crawler directory)
./indexer ../data/fakeDir ../data/indexLetters10

# invalid indexFile (non-existent path) 
./indexer ../data/letters10 ../data/newFile

# invalid indexFile (read-only directory) 
./indexer ../data/letters10 ../data/letters0

# invalid indexFile (existing, read-only file) 
./indexer ../data/letters10 ../data/readOnlyFile

########### VALID ARGUMENTS ############
# Valid arguments; no valgrind
./indexer ../data/letters10 ../data/indexLetters10

# Valid arguments with valgrind
valgrind --leak-check=full --show-leak-kinds=all ./indexer ../data/wikipedia1 ../data/indexWikipedia1

# Use indexTest; no valgrind
./indexTest ../data/indexWikipedia1 ../data/newIndexWikipedia1

# indexTest; with valgrind
valgrind --leak-check=full --show-leak-kinds=all ./indexTest ../data/indexLetters10 ../data/newIndexLetters10
