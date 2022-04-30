/* 
 * indexer.h - header file for TSE indexer module
 *
 * The TSE indexer is a standalone program that reads the document files 
 *   produced by the TSE crawler, builds an index, and writes that index 
 *   to a file. Its companion, the index tester, loads an index file produced
 *   by the indexer and saves it to another file.
 * 
 * usage: indexer pageDirectory indexFilename
 *
 * Andres Ibarra, February 2022
 */

#include "webpage.h"
#include "index.h"
#include "hashtable.h"
#include "counters.h"

/**************** indexBuild ****************/
/*
 * builds an in-memory index from webpage files it finds in the pageDirectory
 * We guarantee:
 *   pageDirectory will be readable
 *   indexFilename will be writeable
 *   index hashtable will be created, mapping normalized words to countersets
 *     holding (docID, count)
 * We assume: 
 *   pageDirectory is verified as created by crawler
 *   pageDirectory is readable
 * Note: 
 *   Exit non-zero if error occurred
 */
void indexBuild(index_t* idx, const char *pageDirectory); 

