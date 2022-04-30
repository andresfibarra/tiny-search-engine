/* 
 * index.h - header file index module for TSE
 *
 * index providing the data structure to represent the in-memory index, and 
 *   functions to read and write index files;
 *
 * Andres Ibarra, February 2022
 */

#include <stdio.h>
#include "hashtable.h"
#include "counters.h"

/***** INDEX STRUCT FUNCTIONS *****/

/***** GLOBAL TYPE *****/
/*
 * index_t struct
 * Wrapper for hashtable (word -> counters (docID -> count))
*/
typedef struct index index_t;

/**************** index_new() ****************/
/*
 * Create a new index object (wrapper for a hashtable)
 * We guarantee:
 *   A hashtable with num_slot will be created
 * We return: 
 *   A pointer to the new index created
 * Note: 
 *   Program will terminate if program out of memory
 */
index_t* index_new(const int num_slots); 

/**************** index_insert() ****************/
/*
 * Insert word/ counter pair whose information is provided into index
 * Caller provides:
 *   An initialized index
 *   A string representing the word 
 *   A valid counters_t struct
 * We guarantee:
 *   The words -> ctr pair will be inserted into the hashtable
 *     backing index idx
 */
void index_insert(index_t* idx, const char* word, counters_t* ctr); 

/**************** index_increment() ****************/
/*
 * Mark the count associated to the word/docID pair as "seen" one more time
 * Caller provides:
 *   An initialized index
 *   A string representing the word 
 *   An int docID greater than 0
 * We guarantee:
 *   If the word is not in the index, it is added to the index with count 1
 *   If the word is in the index, the associated count is incremented by 1
 * Note:
 *   Nothing happens if any invalid arguments provided
 */
void index_increment(index_t* idx, const char* word, const int docID); 

/**************** index_get() ****************/
/* 
 * Getter for the counters_t struct in idx associated to the provided key
 * Implemented as a wrapper for hashtable_find
 * Caller provides:
 *   A valid index idx
 *   A string key whose associated counters_t will be returned
 * We return: 
 *   The counters_t in idx for the given key
 *   NULL if unsuccessful
 * We assume: 
 *   Idx is valid and initialized
 */
counters_t* index_get(index_t* idx, const char* key); 

/**************** index_delete() ****************/
/* 
 * Deletes index by calling hashtable_delete
 * Caller provides:
 *   Initialized index idx
 * We guarantee:
 *   All memory from idxwill be freed
 */
void index_delete(index_t* idx);

/**************** index_saveToFile() ****************/
/* 
 * Prints index to file in format "word docID count [docID count]...", 
 *   as specified in REQUIREMENTS.md
 * Caller provides:
 *   Valid index
 *   Path to writeable file indexFilename 
 * We guarantee:
 *   File will be closed at end
 *   Index information will be printed to file whose path is provided as 
 *     specified in REQUIREMENTS.md
 * Note: 
 *   Error message printed if file is unable to be opened for writing
 */
void index_saveToFile(index_t* idx, const char* indexFilename);

/**************** index_load() ****************/
/* 
 * Create index object with information in indexFilename; used by indexTest 
 * Caller provides: 
 * We assume: 
 *   indexFilename is path to a file written by indexer
 *   File opened is in the format specified in REQUIREMENTS.md
 * We do: 
 *   Parse file specified by path, creating an index object with the 
 *     corresponding information
 * We guarantee:
 *   No memory leaks
 *   File will be closed at end
 * Caller is responsible for: 
 *   Later freeing memory allocating for index by calling index_delete
 * We return:
 *   pointer to index_t struct created
 *   NULL if file unable to be read
 */
index_t* index_load(const char* indexFilename); 

