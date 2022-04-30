/* 
 * index.c - index helper module for TSE
 *
 * see index.h for more information.
 *
 * Andres Ibarra, February 2022
 */

#include <stdio.h>
#include <string.h>
#include "index.h"
#include "mem.h"
#include "hashtable.h"
#include "counters.h"
#include "file.h"
#include "pagedir.h"

/************** GLOBAL TYPE ***************/
typedef struct index {
    hashtable_t* ht;
} index_t;


/********* STATIC FUNCTIONS ************/

//Static function prototypes. Functions are defined below
static void counterDeleteHelper(void *ctr); 
static hashtable_t* getIndexHashtable(index_t *idx); 
static void saveHashtable(void* indexFile, const char* word, void* ctr); 
static void saveCounters(void* indexFile, const int docID, const int count);

/**************** counterDeleteHelper() ****************/
/*
 * Helper function to delete counter object in index  
 * Called by index_delete as the itemDelete specified in hashtable_delete
 */
static void counterDeleteHelper(void *ctr) 
{
    counters_t* toDelete = ctr;
    counters_delete(toDelete);
}

/**************** getIndexHashtable() ****************/
/*
 * Getter for hashtable in index
 */
static hashtable_t* getIndexHashtable(index_t* idx)
{
    return (idx->ht); 
}

/**************** saveHashtable() ****************/
/*
 * Called as the itemfunc in hashtable_iterate within index_saveToFile
 * We do:
 *   Prints index to file in format "word docID count [docID count]...""
 *     as specified in REQUIREMENTS.md. 
 *   Calls saveCounter() to print the docID count pairs
 * Caller provides:
 *   A file indexFile
 *   A string word
 *   A pointer to a counter_t struct
 * We assume: 
 *   ctr is a valid counters struct
 */
static void saveHashtable(void* indexFile, const char* word, void* ctr)
{
    FILE* fp = indexFile;       //cast void* to file 
    counters_t* ctrCpy = ctr;   //cast void* to counters_t*

    fprintf(fp, "%s ", word); 
    counters_iterate(ctrCpy, fp, saveCounters); 
    fprintf(fp, "\n"); 
}

/**************** saveCounters() ****************/
/*
 * Called as the itemfunc in counters_iterate within saveHashtable
 * We do:
 *   Prints docID count pairs to index file as specified in REQUIREMENTS.md
 * Caller provides:
 *   A file indexFile
 *   A string word
 *   A pointer to a counter_t struct
 * We assume: 
 *   ctr is a valid counters struct
 */
static void saveCounters(void* indexFile, const int docID, const int count)
{
    FILE* fp = indexFile;       //cast void* to file
    fprintf(fp, "%d %d ", docID, count);
}


/************** FUNCTIONS ***************/

/**************** index_new() ****************/
/* see index.h for description */
index_t* index_new(const int num_slots) 
{
    index_t* idx = mem_assert(malloc(sizeof(index_t)), "Creating index");
    hashtable_t* ht = hashtable_new(num_slots);
    idx->ht = ht;
    return idx;
}

/**************** index_insert() ****************/
/* see index.h for description */
void index_insert(index_t* idx, const char* word, counters_t* ctr) 
{
    hashtable_insert(getIndexHashtable(idx), word, ctr); 
}

/**************** index_increment() ****************/
/* see index.h for description */
void index_increment(index_t* idx, const char* word, const int docID) 
{
    // check for invalid arguments
    if (idx != NULL && word != NULL && docID > 0) {
        counters_t* ctr = hashtable_find(idx->ht, word); 
        //If word not in index yet, add it
        if (ctr == NULL) {
            ctr = counters_new(); 
            counters_add(ctr, docID); 
            hashtable_insert(idx->ht, word, ctr); 
        } else {
            counters_add(ctr, docID); 
        }
    }
}

/**************** index_get() ****************/
/* see index.h for description */
counters_t* index_get(index_t* idx, const char* key)
{
    if (key != NULL) {
        hashtable_t* ht = getIndexHashtable(idx); 
        if (ht != NULL) {
            return hashtable_find(ht, key); 
        } else {
            return NULL; 
        }
    } else {
        return NULL; 
    }
}

/**************** index_delete() ****************/
/* see index.h for description */
void index_delete(index_t* idx) 
{
    hashtable_delete(getIndexHashtable(idx), counterDeleteHelper);
    free(idx); 
}

/**************** index_saveToFile() ****************/
/* see index.h for description */
void index_saveToFile(index_t* idx, const char* indexFilename)
{
    FILE* fp = fopen(indexFilename, "w"); 
    if (fp == NULL) {
        fprintf(stderr, "Unable to write to %s\n", indexFilename); 
    }

    hashtable_iterate(getIndexHashtable(idx), fp, saveHashtable); 
    fclose(fp); 
}

/**************** index_load() ****************/
/* see index.h for description */
index_t* index_load(const char* indexFilename)
{
    FILE* fp = fopen(indexFilename, "r"); 
    if (fp == NULL) {
        fprintf(stderr, "Unable to read %s\n", indexFilename); 
        return NULL; 
    }

    index_t* idx = index_new(file_numLines(fp) * 2);

    char* word; 
    while ((word = file_readWord(fp)) != NULL) {
        int docID; 
        int count;
        counters_t* ctr = counters_new(); 

        while(fscanf(fp, "%d %d ", &docID, &count) == 2){
			counters_set(ctr, docID, count);
		}
        index_insert(idx, word, ctr); 
		free(word); 
    }

    fclose(fp); 
    return idx; 
} 
