/* 
 * queryScore.c - TSE 'querier' module
 *
 * see queryScore.h for more information.
 * 
 * usage: ./querier pageDirectory indexFilename
 *
 * Andres Ibarra, February 2022
 */

 #include <stdlib.h>
 #include <stdio.h>
 #include <string.h>
 #include <stdbool.h>
 #include "queryScore.h"
 #include "file.h"


/**************** queryScore_new() ****************/
/* see queryScore.h for description */
queryScore_t* queryScore_new(const int docID, const int score)
{
  queryScore_t* qs = malloc(sizeof(queryScore_t));
  if (qs == NULL) {
      return NULL;
  }
  qs->docID = docID;
  qs->score = score;
  return qs; 
}

/**************** queryScore_fetchURL() ****************/
/* see queryScore.h for description */
bool queryScore_fetchURL(queryScore_t *qs, const char* crawlerDirectory)
{
  if (qs != NULL) {
    char* docPath = malloc(strlen(crawlerDirectory) + sizeof(qs->docID) + 1); 
    sprintf(docPath, "%s/%d", crawlerDirectory, qs->docID); 
    FILE* fp = fopen(docPath, "r"); 
    if (fp != NULL) {
      qs->url = file_readLine(fp); 

      fclose(fp); 
      free(docPath); 
      return true; 
    } else {
      free(docPath); 
      return false; 
    }
  } else {
    return false; 
  } 
}

/**************** queryScore_delete() ****************/
/* see queryScore.h for description */
void queryScore_delete(queryScore_t* qs)
{
      if (qs != NULL) {
        if (qs->url != NULL) {
            free(qs->url);
        }
        free(qs);
    }
}