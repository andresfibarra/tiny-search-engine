/* 
 * crawler.c - TSE 'crawler' module
 *
 * see crawler.h for more information.
 * 
 * usage: crawler seedURL pageDirectory maxDepth
 *
 * Andres Ibarra, February 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "crawler.h"
#include "mem.h"
#include "webpage.h"
#include "hashtable.h"
#include "bag.h"
#include "pagedir.h"

// Function prototype declarations. Functions are defined below
static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth);
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);

/**************** main() ****************/
int main(const int argc, char* argv[])
{
  
  char* seedURL =  NULL; 
  char* pageDirectory = NULL; 
  int maxDepth; 
  
  parseArgs(argc, argv, &seedURL, &pageDirectory, &maxDepth);

  crawl(seedURL, pageDirectory, maxDepth); 
}

/**************** parseArgs() ****************/
/* see crawler.h for description */
static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth)
{
  int depthLim = 12; 

  //Check number of arguments
  if (argc != 4) {
    fprintf(stderr, "%s: usage: seedURL pageDirectory maxDepth\n", argv[0]); 
    exit(1); 
  }

  //normalize URL
  char* normalized = normalizeURL(argv[1]); 
  if (normalized == NULL) {
    fprintf(stderr, "%s: error normalizing URL\n", argv[0]); 
    exit (2); 
  }
  *seedURL = mem_assert(malloc(strlen(normalized)+1), "mallocing seedURL"); 
  strcpy(*seedURL, normalized); 
  free(normalized); 

  if (seedURL == NULL ) {
    fprintf(stderr, "%s: error creating normalized seed url\n", argv[0]); 
    exit(3); 
  }

  //Validate it is an internal URL
  if (!isInternalURL(*seedURL)) {
    fprintf(stderr, "%s: provided url not internal. Internal url ",  argv[0]);
    fprintf(stderr, "must start with http://cs50tse.cs.dartmouth.edu/tse/\n"); 
    exit(4); 
  }

  //initialize pageDirectory
  if (!pagedir_init(argv[2])) {
    fprintf(stderr, "%s: error initializing page directory\n", argv[0]); 
    exit(5); 
  } else {
    *pageDirectory = mem_assert(malloc(strlen(argv[2])+1), "mallocing pageDirectory"); 
    strcpy(*pageDirectory, argv[2]); 
  }

  //check maxDepth
  if (sscanf(argv[3], "%d", maxDepth) != 1 || *maxDepth < 0 || *maxDepth > depthLim) {
    fprintf(stderr, "%s: maxDepth out of bounds\n", argv[0]); 
    exit(6); 
  }
}


/**************** crawl() ****************/
/* see crawler.h for description */
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth)
{
  const int htSize = 200; 
  int id = 1; 

  char* seedCpy = mem_assert(malloc(strlen(seedURL) +1), "copying seed URL"); 
  strcpy(seedCpy, seedURL); 

  //initialize data structures
  bag_t* toVisit = bag_new(); 
  hashtable_t* seen = hashtable_new(htSize); 

  webpage_t* seed = webpage_new(seedCpy, 0, NULL);

  //add seed to data structures
  bag_insert(toVisit, seed); 
  hashtable_insert(seen, seedCpy, ""); 

  webpage_t* current = NULL; 

  //crawl pages while bag is not empty
  while ((current = bag_extract(toVisit)) != NULL) {
    if (webpage_fetch(current)) {
      printf("%d  Fetched: %s\n", webpage_getDepth(current), webpage_getURL(current)); 
      pagedir_save(current, pageDirectory, id);
      id++; 

      if (webpage_getDepth(current) < maxDepth) {
        printf("%d  Scanning: %s\n", webpage_getDepth(current), webpage_getURL(current));
        pageScan(current, toVisit, seen); 
      }
    } else {
      fprintf(stderr, "Unable to fetch %s\n", webpage_getURL(current)); 

      exit(7);
    }
    webpage_delete(current); 
  }

  //free memory
  bag_delete(toVisit, webpage_delete); 
  hashtable_delete(seen, NULL); 
  free(seedURL); 
  free(pageDirectory); 
}

/**************** pageScan() ****************/
/* see crawler.h for description */
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen) 
{
  char* next = NULL; 
  int pos = 0; 

  while ((next = webpage_getNextURL(page, &pos))) {
    printf("%d  Found: %s\n", webpage_getDepth(page), next);
    if (isInternalURL(next)) {
      if (hashtable_insert(pagesSeen, next, "")) {
        bag_insert(pagesToCrawl, webpage_new(next, webpage_getDepth(page) + 1, NULL)); 
        printf("%d  Added: %s\n", webpage_getDepth(page), next); 
      } else {
        printf("%d IgnDupl: %s\n", webpage_getDepth(page), next); 
        free(next); 
      }
    } else {
      printf("%d   IgnExtrn: %s\n", webpage_getDepth(page), next);
      free(next); 
    }
  }
}
