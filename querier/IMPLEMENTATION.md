# CS50 TSE Indexer

## Implementation Spec

In this document we reference the Requirements Spec and Design spec for the Query Module of the Tiny Search Engine

In this document, the major sections are:
* Data structures
* Control flow: pseudo code for overall flow, and for each of the functions
  * querier
  * queryScore
* Function prototypes
  * querier
  * queryScore
* Error handling and recovery
* Testing plan

## Data structures

We create a _queryScore_ struct, which has a _docID_ field, a _score_ field, and a _url_ field 

## Control flow - Querier

### main

The main function calls parseArgs, loads the index, calls query, then exits zero. 

### query

Reads queries from stdin until EOF is reached. Then validates, performs, and prints queries. Pseudocode: 

```
While there is another line to be read
  if verifyQuery returns true (the query is clean)
    Create a copy of query to modify
    if the query was not empty
      call createWordsArray on queryCpy to create an array of strings
      createScores
      printScores
```

### verifyQuery

Verifies that the query is valid as specified in REQUIREMENTS.md. Pseudocode: 

```
Make alpha chars in query all lower case
if isOnlyAlpha returns false
  Print error message and exit
Create copy of query to modify
Call createWordsArray on queryCpy to create an array of strings
Loop over array of words
  if the first or last words are an operator
    Print error message and return false
  if two operators are adjacent, return false
    Print error message and return false
if passes all invalid cases, return true
```

### createScores

Creates the scores for documents containing the query words, scoring as specified in REQUIREMENTS.md. Pseudocode:

```
Create a new, empty counters_t called scores
Set counters_t temp to NULL
Loop over words in query
  if current word is "and"
    Skip to next iteration
  else if current word is "or"
    scores v temp
    temp = NULL
  else if temp == NULL (beginning of an and sequence)
    temp = counter in index for current word
  else (continuing an and sequence)
    Set counters_t curr = counter in index for current word
    temp ^ curr
    Clean up temp (remove nodes with score = 0)
scores v temp
return scores
```

Note: "scores" is the total scores, "temp" is a temporary tracker for scores in andSequences
ctrA v ctrB denotes union between ctrA and ctrB; ctrA ^ ctrB denotes intersection between the two

### printScores

Prints out information about documents found by querier. Pseudocode:

```
If no documents found by query, print that
else 
  Print how many documents matched
  Create an array of queryScores for matching documents
  Loop over each queryScore
    queryScore_fetchURL for that score
    Print the score, docID, and url
```

### helper/ utility functions

`numWordsInQuery` counts and returns the number of words in the query. Pseudocode:

```
Create boolean onWord to remember if char being looked at is the first after a word
Loop through the charactres in the query
  If current character is an alpha char
    set onWord to true
  else 
    if onWord
      incremement wordCount
      set onWord to false
Return wordCount
```

`createWordsArray` parses the string for words and puts the words into an array, which it returns. Pseudocode:

```
Allocate memory for wordsArray
Loop over characters in string
  if current char is a null character
    break
  if current char is an alpha and this is the start of a new word
    Put a pointer to this char in the string in wordsArray
  else if this is the first non alpha char after a word
    Replace current char with a null char
Return wordsArray
```

`deleteWordsArray` deletes the given wordsArray and frees associated memory

`countersUnion` performs a set union between two countersets by passing off work to `unionHelper`

`unionHelper` sets the new score to be the sum of the two scores

`countersIntersection` performs a set intersection between two countersets by passing off work to `intersectionHelper`

`intersectionHelper` sets the new score to be the lower score between the two counterset's scores

`copyCounters` returns a cleaned up copy of the provided counterset by passing off work to `copyCountersHelper`

`copyCountersHelper` adds a counternode to the counterset if the score is greater than zero, therefore, creating a "cleaned up" copy

`createScoresArrray` returns an array of _queryScores_ sorted in decreasing order for use when printing scores. Pseudocode:

```
Create an unsorted array of queryScores by calling counters_iterate with createScoresArrayHelper as the itemfunc
Loop over each element in the array
  int max keeps track of the index of the biggest score seen so far
  int maxScore keeps track of the biggest score seen so far
  Loop over each element in the array
    if current element != NULL and current score > maxScore
      max = current index
      maxScore = current score
  Next element in sorted array = element in unsorted array at index max
  Element in unsorted array at index max = NULL
  Set max = 0
Delete unsorted array
Return sorted array
```

`createScoresArrayHelper` helps create the unsorted array of _queryScores_ by adding a _queryScore_ with the information at the current counternode to the array

`deleteScoresArray` deletes an array of _queryScores_ by deleting each element in the array with `queryScore_delete`, then freeing the memory used for the array itself

`getCounterSize` returns the number of nodes in the counter. Passes off work to `sizeHelper`

`sizeHelper` increments a size variable whenever a new counternode is visited

`isOperator` returns true if the word is an operator (either "and" or "or"), false otherwise

`isOnlyAlpha` returns true if the string has no bad characters (non alpha or whitespace chars), false otherwise

`printCleanQuery` prints out the preprocessed, clean, query, as specified in REQUIREMENTS.md

`prompt` prints a prompt iff stdin is a terminal

## Control flow - QueryScore

### queryScore_new

Returns a pointer a new _queryScore_, initialized with the provided docID and score

### queryScore_fetchURl

Adds url to the _queryScore_ sstruct by reading the first line of the corresponding crawler file

### queryScore_delete

Deletes a _queryScore_ struct and associated memory

## Function prototypes

### querier

Detailed descriptions of each function are provided as a paragraph comment in `querier.h `. In the case of static functions, the paragraph comment can be found in `querier.c`

```c
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
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
```

### queryScore

Detailed descriptions of each function are provided as a paragraph comment in `queryScore.h `

```c
queryScore_t* queryScore_new(const int key, const int score);
bool queryScore_fetchURL(queryScore_t *qs, const char* crawlerDirectory); 
void queryScore_delete(queryScore_t *qs);
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by mem_assert function, which result in a message printed to stderr and a non-zero exit status. We anticipate out-of-memory errors to be rare and thus allow the program to crash in this way.

All code uses defensive-programming tactics to catch and exit, e.g., if a function receives bad parameters.

That said, certain errors are caught and handled internally: for example, many functions will simply return NULL or ignore faulty inputs

## Testing plan

The testing plan for this implementation is the same a that specified in the design specification. 
`testing.sh` shall use `../data/wikipedia1` and `../data/indexWikipedia1`, and shall pipe input from files in the `./queryTest` directory
The query results should be compared by eye to corresponding files in the `./expectedResults` directory, where they are written in (unsorted) form, `[query]: [(docID, score), ...]`