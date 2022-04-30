# CS50 TSE Querier

## Design Spec

The TSE Querier is a standalone program that reads the index file produced by the TSE Indexer, and page files produced by the TSE Querier, and answers search queries submitted via stdin.

In this document we reference the Requirements SPec for the Querier Module of teh Tiny Search Engine

In this document, the major sectiosn are: 
* User interface
* Inputs and outputs
* Functional decomposition into modules
* Pseudo code for logic/ algorithmic flow
* Major data structures
* Testing plan

### User Interface

The querier's only interface with the user is on the command-line; it must always have two arguments.

```
./querier pageDirectory indexFilename
```

For example, if `letters` is a pageDirectory in ../data, and `indexLetters` is the corresponding index file, then

```
./querier ../data/letters ../data/indexLetters
```

### Inputs and Outputs

**Input:** The querier reads queries from stdin, one line at a time

**Output:** The querier will print to stderr any errors found. If the query is successful, the querier will print to stdout the information about the query reuslts

### Functional decomposition into modules

We anticipate the following modules or functions: 

*main*, which controls the overarching logic flow
*parseArgs*, which validates and unpacks the command-line args
*query*, which reads the query and controls the cleaning, parsing, and printing of it
*verifyQuery*, which ensures the query is clean, as described in REQUIREMENTS.md
*createScores*, which creates the scores, for each document matching the query
*printScores*, which prints out the documents and related information in descending order according to their scores

We also anticipate Querier to utilize the common module. In particular, 
*index* to load an index_t from indexFile
*word* to make the query all lower case

### Pseudocode for logic/algorithmic flow

The querier will run as follows:

```
Validate and unpack command-line arguments
Load an index struct from indexFilename
Call query
```

where *query*:
```
while there are still queries from stdin
  Verify that the query is clean by calling verifyQuery
    Reprint the clean query
    Call CreateScores
    Print the scores by calling printScores
```

where *verifyQuery*:
```
Normalize word to all lower case
If there are any non alpha characters in the query, print an error
If there are any bad operator cases as described in REQUIREMENTS.md, print an error
```

where *createScores*:
```
Loops over words, scoring as required by Backus-Nour Form
Returns a counters_t mapping docID -> score
```

where *printScores*:
```
if no documents match the query, say so
else
  Sort documents in descending order according to their scores
  Print the document information
```

### Major data structures

* *index*: mapping word -> (docID, count) (view `index.h` for more information)
* *queryScore*: struct keeping track of the document score, docID, and url

### Testing plan

*Unit testing*: To unit test, we shall test as the program is built. For example, once the code to clean the query is written, the program will be run with bad cases to test that the program acts correctly. These tests will not be in the github repository. 

*Integration testing*: A program *queryTest* will serve as our tester for querier. It will pipe queries from files and will:
1. Test for invalid command-line parameters (wrong number of argument, invalid or unreadable files)
2. Test on queries with invalid queries (bad characters or invalid operator cases)
3. Test on blank queries
4. Test on queries that shouldn't match anything
5. Test on single queries, verified by eye when compared to the relevant index file
6. Test on queries of two query words, no operator or separated by an and, verified by eye when compared to the relevant index file 
7. Test on queries of two words, separated by an or, verified by eye when compared to the relevant index file 
8. Test on longer queries, verified by eye when compared to the relevant index file 
9. Run on equivalent, but differently ordered queries to check consistency
10. Run queries with valgrind to ensure no memory leaks
 