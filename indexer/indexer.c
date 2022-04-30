/* 
 * indexer.c - TSE 'indexer' module
 *
 * see indexer.h for more information.
 * 
 * usage: indexer pageDirectory indexFilename
 *
 * Andres Ibarra, February 2022
 */

#include <stdio.h>
#include <string.h>
#include "indexer.h"
#include "mem.h"
#include "hashtable.h"
#include "counters.h"
#include "file.h"
#include "pagedir.h"
#include "index.h"
#include "webpage.h"
#include "word.h"

//Function prototype declarations. Functions are defined below
static void parseArgs(const int argc, char* argv[],
                      char** pageDirectory, char** indexFilename);
void indexBuild(index_t* idx, const char *pageDirectory); 
static void indexPage (webpage_t* page, int docID, index_t* idx);

/**************** main() ****************/
int main(const int argc, char* argv[]) 
{
  char* pageDirectory =  NULL; 
  char* indexFilename = NULL; 
  
  parseArgs(argc, argv, &pageDirectory, &indexFilename);

  //Create new index 
  index_t* idx = index_new(900);
  if (idx == NULL) {
      fprintf(stderr, "%s: index couldn't be created\n", argv[0]);
      return 4; 
  }

  indexBuild(idx, pageDirectory); 

  index_saveToFile(idx, indexFilename);

  //clean up
  index_delete(idx); 
  free(pageDirectory); 
  free(indexFilename); 

  return 0; 
}

/**************** parseArgs() ****************/
/*
 * Validate and unpack command-line args
 * We guarantee:
 *   pageDirectory will be readable
 *   indexFilename will be writeable
 * Note: 
 *   Exit non-zero if error occurred
 *   Allocates mmemory for pageDirectory and indexFilename; 
 *     end of indexBuild responsible for freeing memory
 */
static void parseArgs(const int argc, char* argv[],
                      char** pageDirectory, char** indexFilename)
{
  //Check number of arguments
  if (argc != 3) {
    fprintf(stderr, "%s: usage: indexer pageDirectory indexFilename\n", argv[0]); 
    exit(1); 
  }

  //Validate that pageDirectory was created by crawler
  char* crawlerFile = mem_assert(malloc(strlen(argv[1]) + strlen(".crawler") +2), "validating pageDirectory"); 
  sprintf(crawlerFile, "%s/.crawler", argv[1]);
  FILE* fp = fopen(crawlerFile, "r"); 
  if (fp == NULL) {
    free(crawlerFile); 
    fprintf(stderr, "%s: %s not a directory created by crawler\n", argv[0], argv[1]);
    exit(2); 
  }
  fclose(fp); 

  //initialize pageDirectory
  *pageDirectory = mem_assert(malloc(strlen(argv[1])+1), "mallocing pageDirectory");
  strcpy(*pageDirectory, argv[1]);  

  //Verify that indexFilename is the pathname of a file that can be written
  fp = fopen(argv[2], "w"); 
  if (fp == NULL) {
    fprintf(stderr, "%s: %s not a writeable file\n", argv[0], argv[2]);
    exit(3); 
  }
  fclose(fp); 

  //initialize indexFilename
  *indexFilename = mem_assert(malloc(strlen(argv[2])+1), "mallocing indexFilename");
  strcpy(*indexFilename, argv[2]);  

  //cleanup
  free(crawlerFile); 
}

/**************** indexBuild() ****************/
/* see indexer.h for description */
void indexBuild(index_t* idx, const char *pageDirectory)
{
  int docID = 1; 

  char* filePath = mem_assert(malloc(strlen(pageDirectory) + (sizeof(docID)) + 1), "creating filePath");
  sprintf(filePath, "%s/%d", pageDirectory, docID); 

  FILE* fp = fopen(filePath, "r"); 
  free(filePath);  

  while (fp != NULL) {
    //create webpage
    webpage_t* page = pagedir_createPage(fp); 
    webpage_fetch(page); 
    
    indexPage(page, docID, idx); 

    //clean up
    fclose(fp); 
    webpage_delete(page); 

    //set up next file
    docID++; 
    char* filePath = mem_assert(malloc(strlen(pageDirectory) + (sizeof(docID)) + 1), "creating filePath");
    sprintf(filePath, "%s/%d", pageDirectory, docID); 
    fp = fopen(filePath, "r"); 
    free(filePath); 
  }
}

/**************** indexPage() ****************/
/*
 * scans a webpage document to add its words to the index
 * We assume: 
 *   indexFilename is a valid, writeable, file
 *   idx is a valid, initialized, index struct
 */
static void indexPage (webpage_t* page, int docID, index_t* idx)
{
  //process one word at a time
  int pos = 0; 
  char* word; 
  while ((word = webpage_getNextWord(page, &pos)) != NULL) {
    if (strlen(word) >= 3) {
      normalizeWord(word); 
      index_increment(idx, word, docID); 
    }

    free(word); 
  }
}
