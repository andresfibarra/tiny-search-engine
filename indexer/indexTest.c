/* 
 * indexTest.c - test module for TSE 'indexer' module
 * usage: indexTest oldIndexFilename newIndexFilename
 * 
 * Parses index from old file and recreates it in a new index_t struct. 
 * Then proceeds to write the index_t struct to newIndexFilename
 * 
 * Assumes oldIndexFilename was created by indexer
 * Minimal error checking and argument validation performed
 *
 * Andres Ibarra, February 2022
 */

 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include "mem.h"
 #include "indexer.h"
 #include "index.h"

/**************** main() ****************/
int main(const int argc, const char *argv[]) 
{
  //parse args
  if (argc != 3) {
    fprintf(stderr, "%s: usage: indexTest oldIndexFilename newIndexFilename\n", argv[0]); 
    return 1; 
  }

  //create local copies of filenames
  char* oldIndexFilename = mem_assert(malloc(strlen(argv[1]) + 1), "malloc oldIndexFilename");
  char* newIndexFilename = mem_assert(malloc(strlen(argv[2]) + 1), "malloc newIndexFilename");
  strcpy(oldIndexFilename, argv[1]); 
  strcpy(newIndexFilename, argv[2]); 

  //build index using the old index file
  index_t* idx = index_load(oldIndexFilename); 

  //reprint to a new index file for comparison
  index_saveToFile(idx, newIndexFilename); 

  //clean up
  free(oldIndexFilename);  
  free(newIndexFilename); 
  index_delete(idx);

  return 0; 
}