/* 
 * pagedir.c - TSE 'pagedir' module
 *
 * see pagedir.h for more information.
 *
 * Andres Ibarra, February 2022
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "webpage.h"
#include "file.h"

/**************** pagedir_init() ****************/
/* see pagedir.h for description */
bool pagedir_init(const char* pageDirectory)
{
  char* testFileName = malloc(strlen(pageDirectory) + strlen(".crawler") + 2);
  sprintf(testFileName, "%s/.crawler", pageDirectory); 

  FILE* dummyFile = fopen(testFileName, "w"); 

  if (dummyFile == NULL) {
        free(testFileName);
        return false;
  }

  // clean up
  fclose(dummyFile); 
  free(testFileName);

  return true;

}

/**************** pagedir_save() ****************/
/* see pagedir.h for description */
void pagedir_save(const webpage_t* page, const char* pageDirectory, 
          const int docID)
{
  char* path = malloc(strlen(pageDirectory) + sizeof(docID) + 1); 
  
  sprintf(path, "%s/%d", pageDirectory, docID);

  FILE *fp = fopen(path, "w");

  fprintf(fp, "%s\n%d\n%s\n", webpage_getURL(page), webpage_getDepth(page),
                                webpage_getHTML(page));

  fclose(fp);
  free(path);
}

/**************** pagedir_createPage() ****************/
/* see pagedir.h for description */
webpage_t* pagedir_createPage(FILE* fp) 
{
  char* url = file_readWord(fp); 
  char* urlCpy = malloc(strlen(url)+1); 
  strcpy(urlCpy, url); 
  char* depthLine = file_readLine(fp); 
  int depth = atoi(depthLine); 

  webpage_t* page = webpage_new(urlCpy, depth, NULL); 

  //cleanup according to contract specified in file.h
  free(url); 
  free(depthLine); 

  return page; 
}