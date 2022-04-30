/* 
 * queryScore.h - header file for TSE querier module
 *
 * This module holds the code for the queryScore struct and associated 
 *   functions
 *
 * Andres Ibarra, February 2022
 */

 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include <stdbool.h>
 #include "counters.h"
 #include "hashtable.h"

#ifndef QUERYSCORE_H
#define QUERYSCORE_H

typedef struct queryScore {
    int docID;
    int score;
    char* url;
} queryScore_t;

#endif

/**************** queryScore_new() ****************/
/*
 * Create a new queryScore struct
 * We return: 
 *   Pointer to the new queryScore; NULL if error (out of memeory)
 * We guarantee:
 *   New queryScore is initialized with the provided docID and score
 *   New queryScore's url field is uninitialized
 * Caller is responsible for:
 *   Later calling queryScore_fetchURL() to fill out url field
 *   Later calling queryScore_delete()
 */
queryScore_t* queryScore_new(const int key, const int score);

/**************** queryScore_fetchURL() ****************/
/*
 * Adds url to the queryScore struct by reading first line of corresponding 
 *   crawler file
 * Caller provides:
 *   QueryScore struct
 *   Path to the crawler created directory
 * We assume: 
 *   crawlerDirectory is a directory created by crawler
 *   qs has an initialized key field
 * We return: 
 *   True if successful
 *   False if qs is null or if tf the crawler file opened is null
 * We guarantee: 
 *   url field in queryScore struct will be filled 
 */
bool queryScore_fetchURL(queryScore_t *qs, const char* crawlerDirectory); 

/**************** queryScore_delete() ****************/
/*
 * Deletes a queryScore struct
 * Caller provides:
 *   queryScore struct
 * We do: 
 *   Free the url in queryScore, then free the struct itself
 */
void queryScore_delete(queryScore_t *qs);
