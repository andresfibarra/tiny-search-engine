/* 
 * querier.c - TSE 'querier' module
 *
 * see querier.h for more information.
 * 
 * NOTE: IN ESSENSE, THIS IS BASICALLY COMPUTING EVERYTHING AS AN OR SEQUENCE
 * 
 * usage: ./querier pageDirectory indexFilename
 *
 * Andres Ibarra, February 2022
 */

#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>
#include <unistd.h>
#include "querier.h"
#include "queryScore.h"
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
void query(index_t* idx, char* pageDirectory); 
bool verifyQuery(char* query); 
int numWordsInQuery(char* query);
char** createWordsArray(char* string, int numWords); 
void deleteWordsArray(char** wordsArray, int numWords); 
counters_t* createScores(index_t* idx, char** queryWords, int numWords);
void countersUnion(counters_t* ctrA, counters_t* ctrB); 
static void unionHelper(void* arg, const int key, const int count); 
void countersIntersection(counters_t* ctrA, counters_t* ctrB); 
static void intersectionHelper(void* arg, const int key, const int count); 
counters_t* copyCounters(counters_t* ctr); 
static void copyCountersHelper(void* arg, const int key, const int count); 
void printScores(counters_t* scores, char* pageDirectory); 
queryScore_t** createScoresArray(counters_t* scores, int size); 
static void createScoresArrayHelper (void* arg, const int key, const int count); 
void deleteScoresArray(queryScore_t** scoresArray, int size);
int getCountersSize(counters_t* ctr); 
static void sizeHelper(void* arg, const int key, const int count); 
static bool isOperator(char* word); 
static bool isOnlyAlpha(char* str); 
static void printCleanQuery(char** queryWords, int numWords); 
static void prompt(void);
int fileno(FILE *stream);

/**************** main() ****************/
int main(const int argc, char* argv[]) 
{
  char* pageDirectory = NULL; 
  char* indexFilename = NULL; 

  parseArgs(argc, argv, &pageDirectory, &indexFilename); 

  /* load index from indexFilename into an internal data structure */
  index_t* idx = index_load(indexFilename); 
  if (idx == NULL) {
    fprintf(stderr, "Error initializing index\n"); 
    exit(4); 
  }

  /* Query time! */
  query(idx, pageDirectory); 


  //clean up
  free(pageDirectory); 
  free(indexFilename); 
  index_delete(idx); 

  return 0; 
}

/**************** parseArgs() ****************/
/* 
 * Validate and unpack command-line args
 * We guarantee:
 *   pageDirectory was created by crawler
 *   indexFilename and pageDirectory will be readable
 * Caller is responsible for: 
 *   Freeing memory for pageDirectory and indexFilename
 * Note: 
 *   Exit non-zero if error occurred
 */
static void parseArgs(const int argc, char* argv[],
                      char** pageDirectory, char** indexFilename)
{
  //Check number of arguemnts
  if (argc != 3) {
    fprintf(stderr, "%s: usage: ./querier pageDirectory indexFilename\n", argv[0]); 
    exit(1); 
  }

  //Validate that pagedirectory was created by crawler
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

  //initialize indexFilename
  *indexFilename = mem_assert(malloc(strlen(argv[2])+1), "mallocing indexFilename"); 
  strcpy(*indexFilename, argv[2]); 

  //Validate that indexFilename is the pathname of a file that can be read
  fp = fopen(*indexFilename, "r"); 
  if (fp == NULL) {
    fprintf(stderr, "%s: %s not a readable file\n", argv[0], argv[2]); 
    exit(3); 
  }
  fclose(fp); 

  //clean up
  free(crawlerFile); 
}

/**************** query() ****************/
/* see querier.h for description */
void query(index_t* idx, char* pageDirectory) 
{
  //read search queries from stdin, one per line, till EOF
  char* query = NULL; 
  prompt(); 
  while ((query = file_readLine(stdin)) != NULL) {
    //clean query
    if (verifyQuery(query)) { //if query is clean, continue
      //print clean query
      char* queryCpy = mem_assert(malloc(strlen(query)+1), "creating queryCpy");
      strcpy(queryCpy, query); 
      //create array of query words)
      int numWords = numWordsInQuery(queryCpy);
      if (numWords != 0) {
        char** queryWords = createWordsArray(queryCpy, numWords);
        printCleanQuery(queryWords, numWords);  
        
        //parse query
        counters_t* scores = createScores(idx, queryWords, numWords); 
        printScores(scores, pageDirectory); 

        //clean up
        deleteWordsArray(queryWords, numWords); 
        counters_delete(scores); 
      }
      free(queryCpy); 
    } 
    free(query); 
    prompt(); 
  }
}

/**************** verifyQuery() ****************/
/* see querier.h for description */
bool verifyQuery(char* query)
{
  normalizeWord(query); 
  if (!isOnlyAlpha(query)) {
    fprintf(stderr, "Error: Bad character in query\n");
    return false; 
  } else {
    //check invalid operator cases

    //create copy of query
    char* queryCpy = mem_assert(malloc(strlen(query)+1), "mallocing queryCpy");
    strcpy(queryCpy, query); 
    //find number of words
    int numWords = numWordsInQuery(queryCpy);

    //create array of char words
    char** queryWords = createWordsArray(queryCpy, numWords); 

    //loop over array of words to check invalid operator cases
    if (queryWords != NULL && numWords != 0) {
      if (isOperator(queryWords[0])) {
        fprintf(stderr, "Error: '%s' cannot be first\n", queryWords[0]);
        return false;  
      } else if (isOperator(queryWords[numWords-1])) {
        fprintf(stderr, "Error: '%s' cannot be last\n", queryWords[numWords-1]); 
        return false; 
      } else {
        //loop over all words, check for adjacent operator
        for (int i = 0; i < numWords; i++) {
          if ((i > 0) && (isOperator(queryWords[i])) && (isOperator(queryWords[i-1]))) {
            fprintf(stderr, "Error: '%s' and '%s' cannot be adjacent\n", queryWords[i-1], queryWords[i]); 
            return false; 
          }
        }
      }
    }

    //clean up
    deleteWordsArray(queryWords, numWords);  
    free(queryCpy); 
  }
  return true;  
}

/**************** numWordsInQuery() ****************/
/* see querier.h for description */
int numWordsInQuery(char* query)
{
  int wordCount = 0; 
  bool onWord = false; 
  for (int i = 0; i < strlen(query)+1; i++) {
    if (isalpha(query[i])) {
      onWord = true; 
    } else {
      if (onWord) {
        wordCount++;
        onWord = false; 
      }
    }
  }

  return wordCount; 
}

/**************** createWordsArray() ****************/
/* see querier.h for description */
char** createWordsArray(char* string, int numWords)
{
  char** wordsArray = calloc(numWords, sizeof(char*));
  int index = 0; 
  bool onWord = false; 
  int len = strlen(string); 

  for (int i = 0; i < len; i++) {
    //slide the word pointer down unil you find a alpha char or hit null
    if (string[i] == '\0') {
      break;
    } 
    if (isalpha(string[i]) && !onWord) { 
      //if this is the start of a new word
      wordsArray[index] = &string[i]; 
      index++; 
      onWord = true; 
    } else if (!isalpha(string[i]) && onWord) { 
      //if on first non alpha char after word
      string[i] = '\0';
      onWord = false; 
    }
  }

  return wordsArray; 
}

/**************** deleteWordsArray() ****************/
/* see querier.h for description */
void deleteWordsArray(char** wordsArray, int numWords) 
{
  free(wordsArray); 
  wordsArray = NULL; 
}

/**************** createScores() ****************/
/* see querier.h for description */
counters_t* createScores(index_t* idx, char** queryWords, int numWords)
{

  counters_t* scores = counters_new(); 
  counters_t* temp = NULL; 
  counters_t* curr = NULL; 

  for (int i = 0; i < numWords; i++) {
    if (strcmp(queryWords[i], "and") == 0) {
      continue; //skip to next word bc of implicit and between two nonoperators
    } else if (strcmp(queryWords[i], "or") == 0) {
      //reached end of of and sequence
      countersUnion(scores, temp); 
      if (temp != NULL) {
        counters_delete(temp); 
        temp = NULL; 
      } else {
        fprintf(stderr, "Error: temp is null\n"); 
      }
    } else if (temp == NULL) {
      //beginning of and sequence
      counters_t* tempCounter = index_get(idx, queryWords[i]); 
      if (tempCounter == NULL) {
        temp = counters_new(); 
      } else {
        temp = copyCounters(tempCounter);
      }
    } else {
      //continuing and sequence
      curr = index_get(idx, queryWords[i]); 
      if (curr == NULL) {
        curr = counters_new(); 
        countersIntersection(temp, curr); 
        counters_t* cleanedUp = copyCounters(temp);
        counters_delete(temp);
        counters_delete(curr); 
        temp = cleanedUp; 
      } else {
        countersIntersection(temp, curr); 
        counters_t* cleanedUp = copyCounters(temp);
        counters_delete(temp);
        temp = cleanedUp; 
      }
    }
  }
  countersUnion(scores, temp); 
  counters_delete(temp); 

  return scores; 
}

/**************** countersUnion() ****************/
/* see querier.h for description */
void countersUnion(counters_t* ctrA, counters_t* ctrB)
{
  counters_iterate(ctrB, ctrA, unionHelper); 
}

/**************** unionHelper() ****************/
/*
 * itemfunc for counters_iterate in countersUnion to sum the scores
 *   Used during or sequences
 * We do:
 *   Set the score for this document id to be the 
 *     previous score + the frequency in that docID 
 *     found in the counters_t we're in
 */
static void unionHelper(void* arg, const int key, const int count)
{
  counters_t* scores = arg; 
  counters_set(scores, key, count + counters_get(scores, key)); 
}

/**************** countersIntersection() ****************/
/* see querier.h for description */
void countersIntersection(counters_t* ctrA, counters_t* ctrB)
{
  //create a copy so as not to modify the counter retrieved from index
  counters_t* cpyCtrB = copyCounters(ctrB); 

  counters_iterate(ctrA, cpyCtrB, intersectionHelper); 
  //merge the other to remove doc's in A but not in B
  counters_iterate(cpyCtrB, ctrA, intersectionHelper); 

  counters_delete(cpyCtrB); 
}

/**************** intersectionHelper() ****************/
/* 
 * itemfunc for counters_iterate in countersIntersection
 * We do:
 *   Set the score for this document id to be the minimum
 *     between the current score and the arg's score
 */
static void intersectionHelper(void* arg, const int key, const int count)
{
  counters_t* scores = arg; 
  if (count < counters_get(scores, key)) {    
    counters_set(scores, key, count); 
  } else {
    counters_set(scores, key, counters_get(scores, key)); 
  }
}

/**************** copyCounters() ****************/
/* see querier.h for description */
counters_t* copyCounters(counters_t* ctr) 
{
  if (ctr == NULL) {
    return NULL; 
  }
  counters_t* cpy = counters_new(); 
  if (cpy == NULL) {
    return NULL; 
  }

  counters_iterate(ctr, cpy, copyCountersHelper); 
  return cpy; 
}

/**************** copyCountersHelper() ****************/
/* 
 * itemfunc for counters_iterate in copyCounters
 * We do:
 *   Add a new node to the counters_t copy being built (provided in the 
 *     argument) if the count is greater than zero, in effect "cleaning
 *     up" the copy being built
 */
static void copyCountersHelper(void* arg, const int key, const int count)
{
  counters_t* cpy = arg; 
  if (count > 0) {
    counters_set(cpy, key, count); 
  }
}

/**************** printScores() ****************/
/* see querier.h for description */
void printScores(counters_t* scores, char* pageDirectory)
{
  int size = getCountersSize(scores); 
  if (size == 0) {
    printf("No documents match\n"); 
  } else {
    printf("Matches %d documents\n", size); 
    queryScore_t** scoresArray = createScoresArray(scores, size);

    for (int i = 0; i < size; i++) {
      queryScore_fetchURL(scoresArray[i], pageDirectory); 
      printf("score %d doc %d: %s\n", scoresArray[i]->score, 
              scoresArray[i]->docID, scoresArray[i]->url); 
    }
    deleteScoresArray(scoresArray, size);  
  }
}

/**************** createScoresArray() ****************/
/* see querier.h for description */
queryScore_t** createScoresArray(counters_t* scores, int size)
{
  queryScore_t** scoresArray = calloc(size, sizeof(queryScore_t*)); 
  for (int i = 0; i < size; i++) {
    scoresArray[i] = NULL; 
  }
  counters_iterate(scores, scoresArray, createScoresArrayHelper); 

  //sort scoresArray in decreasing order by creating a parallel array
  queryScore_t** sortedScores = calloc(size, sizeof(queryScore_t*)); 
  int max; 
  int maxScore; 
  for (int i = 0; i < size; i++) {
    max = 0; 
    maxScore = 0; 
    for (int j = 0; j < size; j++) {
      if (scoresArray[j] != NULL && scoresArray[j]->score > maxScore) {
        max = j; 
        maxScore = scoresArray[j]->score; 
      }
    }
    sortedScores[i] = scoresArray[max]; 
    scoresArray[max] = NULL; 

    max = 0; 
  }
  deleteScoresArray(scoresArray, size); 

  return sortedScores; 
}

/**************** createScoresArrayHelper() ****************/
/*
 * itemfunc for counters_iterate 
 *   used in createScoresArray
 * We do:
 *   Create a new queryScore from each visited counternode
 *   Add the new queryscore to the array (provided as the arg)
 */
static void createScoresArrayHelper (void* arg, const int key, const int count)
{
  queryScore_t** scoresArray = arg; 
  queryScore_t* curr = queryScore_new(key, count); 
  int i = 0; 
  //find first open slot
  while (scoresArray[i] != NULL) {
    i++; 
  }
  scoresArray[i] = curr; 
}

/**************** deleteScoresArray() ****************/
/* see querier.h for description */
void deleteScoresArray(queryScore_t** scoresArray, int size)
{
  for (int i = 0; i < size; i++) {
    queryScore_delete(scoresArray[i]); 
  }
  free(scoresArray); 
}

/**************** getCountersSize() ****************/
/* see querier.h for description */
int getCountersSize(counters_t* ctr) {
    int size = 0;
    counters_iterate(ctr, &size, sizeHelper);
    return size;
}

/**************** sizeHelper() ****************/
/*
 * itemfunc for counters_iterate to get the size of the counter
 *   Used by by getCountersSize
 * We do:
 *   Increment size variable (provided by arg) whenever a new counternode
 *     is reached
 */
static void sizeHelper(void* arg, const int key, const int count) {
    int* size = arg;
    (*size)++;
}

/**************** isOperator() ****************/
/*
 * Returns if the word is an operator
 * Caller provides:
 *   valid string, all lowercase, with no leading or trailing white space
 * We return: 
 *   true if the string is either "and" or "or"
 *   false if string is anything else
 */
static bool isOperator(char* word) 
{
  return ((strcmp(word, "or") == 0) || (strcmp(word, "and") == 0)); 
}

/**************** isOnlyAlpha() ****************/
/*
 * Checks to see if there are no bad characters in string str
 * Caller provides:
 *   Null terminated string word
 * We return: 
 *   true if only alphabet characters and white space in provided string
 *   false if else
 */
static bool isOnlyAlpha(char* str) 
{
  for (int i = 0; str[i]; i++) {
    if ((isalpha(str[i]) == 0) && !isspace(str[i])) {
      return false; 
    }
  }
  return true; 
}

/**************** printCleanQuery() ****************/
/*
 * Prints out the preprocessed, clean, query
 * Caller provides:
 *   An array of char pointers to the words of the normalized query
 * We do: 
 *   Print preprocessed query
 */
static void printCleanQuery(char** queryWords, int numWords)
{
  printf("Query: "); 
  for (int i = 0; i < numWords; i++) {
    printf("%s ", queryWords[i]); 
  }
  printf("\n");
}


/**************** prompt() ****************/
/*
 * Print a prompt iff stdin is a tty (terminal)
 */
static void prompt(void)
{
  if (isatty(fileno(stdin))) {
    printf("Query? ");
  }
}