/* 
 * querier.h - header file for TSE querier module
 *
 * The TSE Querier is a standalone program that reads the index file produced 
 * by the TSE Indexer, and page files produced by the TSE Querier, and 
 * answers search queries submitted via stdin.
 * 
 * usage: ./querier pageDirectory indexFilename
 *
 * Andres Ibarra, February 2022
 */

#include "queryScore.h"
#include "index.h"
#include "counters.h"

/**************** query() ****************/
/*
 * Driver for querier module, which cleans query then parses and performs query
 * We do:
 *   Read input from user and calls function to preprocess the query
 *   Call functions to perform and print the query
 * Caller provides:
 *   Valid and initialized index struct
 *   Readable pageDirectory created by crawler
 */
void query(index_t* idx, char* pageDirectory); 

/**************** verifyQuery() ****************/
/*
 * Verifies that the query is valid
 * Caller provides:
 *   Null terminated string query
 * We return: 
 *   true if the query is valid
 *   false if query is invalid according to requirements found in 
 *     REQUIREMENTS.md (bad character, operator at beginning or end, two 
 *     operators next to each other)
 */
bool verifyQuery(char* query); 

/**************** numWordsInQuery() ****************/
/*
 * Finds number of words in the query. A word is defined as one or more
 *   alphabet characters surrounded by whitespace or a null character
 * Caller provides:
 *   valid string, all lowercase, with no leading or trailing white space
 * We assume: 
 *   There are no bad characters in the query string (no non-alpha or non
 *     white space characters)
 * We return: 
 *   Number of words found in query
 */
int numWordsInQuery(char* query); 

/**************** createWordsArray() ****************/
/*
 * Parses the string for words. Puts the words into an array
 * Caller provides:
 *   Valid string with no bad characters
 *   Valid integer corresponding to the number of words in the string
 * We do: 
 *   Add a pointer to the beginning of each word in string and a null 
 *     character at the end of the word
 *   Place that pointer in the returned array
 * We return:
 *   Array of size numWords of the words found in string
 *   NULL on an error
 * Caller is responsible for:
 *   Later calling deleteWordsArray
 * Note: 
 *   Caller should provide copy of the original string, so as not to 
 *     modify it
*/
char** createWordsArray(char* string, int numWords); 

/**************** deleteWordsArray() ****************/
/*
 * Delete provided wordsArray and frees associated memory
 * Caller provides:
 *   Array of char* pointers wordsArray, created by createWordsArray
 *   The number of slots in wordsArray, numWords
 * We do:
 *   Free memory allocated in createWordsArray
 */
void deleteWordsArray(char** wordsArray, int numWords); 

/**************** createScores() ****************/
/*
 * Creates the "scores" for matching documents containing the query words
 * Caller provides:
 *   Valid index struct mapping word -> (docID, freq) 
 *   Array of pointers to the query words, as created by createWordsArray
 *   The number of slots in the queryWords array
 * We do: 
 *   Iterate over documents
 *   Sum up the freq's of the query words in these documents to create score
 * We return:
 *   Counters_t mapping docID -> score
 * Caller is responsible for:
 *   Later deleting the returned counters_t by calling counters_delete
 */
counters_t* createScores(index_t* idx, char** queryWords, int numWords); 

/**************** countersUnion() ****************/
/*
 * set union between two countersets. Used in or sequence when creating scores
 *   Wrapper for calling counters_iterate, with unionHelper as the 
 *   provided itemfunc
 * Caller provides:
 *   Two counter sets
 * We do:
 *   Perform a set union between ctrs A and B, as specified by REQUIREMENTS.md
 *   Add sum the scores for each docID
 * Note:
 *   ctrA is modified, ctrB remains the same
 */
void countersUnion(counters_t* ctrA, counters_t* ctrB); 


/**************** countersIntersection() ****************/
/*
 * set intersection between two countersets. Used in an andSequence when 
 *   creating scores
 *   Passes off work to counters_iterate, with intersectionHelper as the
 *   provided itemfunc
 * Caller provides:
 *   Two counter sets
 * We do: 
 *   Set the scores of each docID to be the minimum between the two
 *   We merge in both directions as to zero out the scores for any documents
 *   that are in one of the provided counter sets but not in the other
 * Caller is responsible for: 
 *   Later creating a copy of ctrA using copyCounters to remove counternodes
 *   with score = 0
 * Note: 
 *   ctrA is modified, ctrB remains the same
 *   To ensure that ctrB remains the same, we create a copy of ctrB so as
 *   not to modify it during the intersection
 */
void countersIntersection(counters_t* ctrA, counters_t* ctrB); 

/**************** copyCounters() ****************/
/*
 * Creates a cleaned up copy of the provided counter set
 * Passes off work to counters_iterate, using copyCounters as the itemfunc
 * Caller provides:
 *   Counterset to be copied
 * We return:
 *   A pointer to a new counters_t with the same information, but stripped of 
 *   any nodes whose scores were zero
 *   Return null if the provided counters_t is null, or if there is an error
 * Caller is responsible for:
 *   Later freeing the returned counters_t
 */
counters_t* copyCounters(counters_t* ctr); 

/**************** printScores() ****************/
/*
 * Prints out information about documents found by the querier
 * Caller provides:
 *   counters_t struct mapping docID -> score
 *   file created by crawler
 * We do:
 *   If no documents matched the query, print out that no documents matched
 *   Else, print the number of matching documents, and the score, docID, and 
 *     url for each
 */
void printScores(counters_t* scores, char* pageDirectory); 

/**************** createScoresArray() ****************/
/*
 * Creates an array of queryScore_t's for use when printing scores
 * Caller provides:
 *   counters_t mapping docID to score
 *   The size of the counters 
 * We do:
 *   Iterate over scores. Create a new queryScore_t for each and add it to 
 *     an unsorted array of queryScore's
 *   Sort the array in descending order
 *   Delete the original, unsorted array
 * We return: 
 *   An array of queryScore_t's, sorted in descending order
 * Caller is responsible for:
 *   Later deleting the returned array by calling deleteScoresArray
 */
queryScore_t** createScoresArray(counters_t* scores, int size); 

/**************** deleteScoresArray() ****************/
/*
 * Delete array of queryScores
 * Caller provides:
 *   An array of queryScore, as created by createScoresArray
 *   The size of the array
 * We do:
 *   Delete each queryScore in the array
 *   Free the memory allocated for the array itself
 */
void deleteScoresArray(queryScore_t** scoresArray, int size);

/**************** getCountersSize() ****************/
/*
 * Gets the size (number of nodes) in the counter
 * Caller provides:
 *   An initialized counters_t
 * We do:
 *   Iterate through ctr, using the helper function sizeHelper to increment
 *     the size variable every time a new node is visited
 * We return: 
 *   The number of nodes in the counter
 *   0 if NULL counterset
 */
int getCountersSize(counters_t* ctr); 

/**************** fileno() ****************/
/*
 * Provided by stdio, but not declared in stdio.h
 */
int fileno(FILE *stream);

