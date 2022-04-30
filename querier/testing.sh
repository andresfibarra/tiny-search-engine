#!/bin/bash
# Testing script for querier
# Run in make test as bash -v testing.sh
#
# Assumes the existence of wikipedia1 and indexWikipedia1 in the 
#   ../data/ directory
# Andres Ibarra, Winter 2022


########### INVALID ARGUMENTS ############
# No arguments
./querier 

# One argument 
./querier ../data/wikipedia1 

# Three or more arguments 
./querier ../data/wikipedia1 ../data/indexWikipedia1 extraArg

# invalid pageDirectory (non-existent path) 
./querier ../data/fakeDir ../data/indexWikipedia1

# invalid pageDirectory (not a crawler directory)
./querier ../data/notCrawlerDir ../data/indexWikipedia1

########### VALID ARGUMENTS ############
# Invalid queries (should only print out error messages)
./querier ../data/wikipedia1 ../data/indexWikipedia1 < ./queryTest/invalidQueries

# Blank query (shouldn't print anything)
./querier ../data/wikipedia1 ../data/indexWikipedia1 < ./queryTest/blankQuery

# Queries that shouldn't match anything
./querier ../data/wikipedia1 ../data/indexWikipedia1 < ./queryTest/queriesNotMatching

# simgle queries
./querier ../data/wikipedia1 ../data/indexWikipedia1 < ./queryTest/singleQueries

# two queryWord queries, explicit or implicit and
./querier ../data/wikipedia1 ../data/indexWikipedia1 < ./queryTest/andQueries

# two queryWord querier, explicit or
./querier ../data/wikipedia1 ../data/indexWikipedia1 < ./queryTest/orQueries

# Longer queries
valgrind --leak-check=full --show-leak-kinds=all ./querier ../data/wikipedia1 ../data/indexWikipedia1 < ./queryTest/longerQueries

# Equivalent queries
./querier ../data/wikipedia1 ../data/indexWikipedia1 < ./queryTest/equivalentQueries
