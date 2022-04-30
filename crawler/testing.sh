#!/bin/bash
# Testing script for crawler.h
# Run in make test as bash -v testing.sh &> testing.out
# Andres Ibarra, Winter 2022

####### Create directories if necessary #######
if [[ ! -d "../data/letters0" && ! -w "../data/letters0" ]];
then
    mkdir "../data/letters0"
    chmod +w "../data/letters0"
fi

if [[ ! -d "../data/letters10" && ! -w "../data/letters10" ]];
then
    mkdir "../data/letters10"
    chmod +w "../data/letters10"
fi

if [[ ! -d "../data/toScrape0" && ! -w "../data/toScrape0" ]];
then
    mkdir "../data/toScrape0"
    chmod +w "../data/toScrape0"
fi

if [[ ! -d "../data/toScrape1" && ! -w "../data/toScrape1" ]];
then
    mkdir "../data/toScrape1"
    chmod +w "../data/toScrape1"
fi

if [[ ! -d "../data/wikipedia0" && ! -w "../data/wikipedia0" ]];
then
    mkdir "../data/wikipedia0"
    chmod +w "../data/wikipedia0"
fi

if [[ ! -d "../data/wikipedia1" && ! -w "../data/wikipedia1" ]];
then
    mkdir "../data/wikipedia1"
    chmod +w "../data/wikipedia1"
fi

####### Invalid number of args #######
./crawler
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters0
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters0 10 extraArg


####### Invalid args #######
# External seedURL
./crawler google.com ../data/letters0 10

# invalid directory
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html fakeDirectory 10

# invalid maxDepth
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters0 100
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters0 -100


####### Unit tests with valid arguments #######
# Letters at depth 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters0 0

# Letters at depth 10
./crawler http://cs50tse.cs.dartmouth.edu/tse/letters/index.html ../data/letters10 10

# toscrape at depth 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toScrape0 0

# toscrape at depth 1 with valgrind
valgrind --leak-check=full --show-leak-kinds=all ./crawler http://cs50tse.cs.dartmouth.edu/tse/toscrape/ ../data/toScrape1 1

# wikipedia at depth 0
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wikipedia0 1

# wikipedia at depth 1
./crawler http://cs50tse.cs.dartmouth.edu/tse/wikipedia/ ../data/wikipedia1 1