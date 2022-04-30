# CS50 TSE Indexer

## Implementation Spec

In this document we reference the Requirements Spec and Design spec for the Index Module of the Tiny Search Engine

In this document, the major sections are:
* Data structures
* Control flow
  * Indexer
  * indexTest
  * Other modules
* Function prototypes
* Error handling and recovery
* Testing plan

## Data structures

We create a _index_ struct, which is a wrapper for a hashtable_t. This hashtable maps words from the crawler created file to counters_t structs, which in turn maps docID's to the number of times this word was seen in that document. Thus, the flow is word -> (docID -> count).

## Control flow - Indexer

The indexer is implemented in `indexer.c`, with four functions

### main

The main function calls `parseArgs` to validate arguments, creates a new index struct with 900 slots to ensure sufficient space, and checks that the index was successfully created. It then calls `indexBuild`, then `index_saveToFile`, and exits zero

### parseArgs

Given arguments from the command line, extract them into the function parameters, return only if successful. Pseudocode:

```
Verify that argc = 3
Validate that pageDirectory was created by crawler
Initialize pageDirectory
Verify that indexFilename is the pathname of a file that can be written
Initialize indexFilename
```

If any trouble is found, print an error to stderr and exit non-zero

### indexBuild

Build an in-memory index from webpage files it finds in the pageDirectory. Pseudocode:

```
Create path
Open file with docID = 1
While there is another file to be opened
  Create and fetch webpage
  Call indexPage on the webpage
  Close file and delete webpage
  Set up the next file
```

### indexPage

Scans a webpage document to add its word to the index. Pseudocode:

```
While there is another word in the webpage
  If word is non trivial (length >= 3)
    Normalize word
    call index_increment
```

## Control flow - IndexTest

Tester for indexer.c functionality. Pseudocode:

```
Create local copies of filenames
Call index_load to build index using old index file
Call index_saveToFile to reprint to new index file
Free memory and return 0
```

## Other modules

### index

`index.c` providing the data structure to represent the in-memory index, `index_t`, and functions to read and write index files. We chose to write this as a separate module in `../common` to encapsulate knowledge on how to read and write index files, expecting future use by Querier. NOTE: the functions to build the index, `indexBuild` and `indexPage` are found in `indexer.c`, not in `index.c`. 

`getIndexHashtable` is a static function that returns the hashtable_t found in the index_t

`index_new` returns a pointer to a new index by creating the specified hashtable and adding the hashtable to the index

`index_insert` adds the word/ counter pair specified into the index by passing off work to `hashtable_insert`

Pseudocode for `index_increment`: 

```
If the word is not in the index yet
  Create counter for it
  Insert it with count = 1 into the index
If the word already exists
  Increment the count in the counter associated to the word
```

`index_get` passes off work to hashtable_find to return the counters_t for the given word

`index_delete` passes off work to hashtable_delete to delete the contents of the index_t struct. It uses `counterDeleteHelper` as the itemFunc. It then deletes the index itself

`counterDeleteHelper` is a static function that, after casting the argument to a counters_t, passes it to `counters_delete` to be deleted

`index_saveToFile` passes off work to the iterators `saveHashtable` and `saveCounters` to print the index to a file as specified in REQUIREMENTS.md

`saveHashtable` is a static function to help with `index_saveToFile`. Pseudocode:

```
Case arguments to correct types
Print the word 
Pass the counter provided to coutners_iterate, with saveCounters as the itemfunc
Print newline character
```

`saveCounters` is a static function to help with `index_saveToFile` and is called by `saveHashable`. It prints the docID count pair to the file.

Pseudocode for `index_load`:

```
Open file
Create new index
While there is another word to be read
  Create a counter
  Scan through the rest of that line in the file in pairs of ints
    Set the counter to have the docID and count found
  Insert that counter into the index
Close the file
Return the created index
```

  Note: `index_load` creates a new index with twice the slots than number of words in order to minimize collisions in the hashtable

### word

`word.c` is a a module providing a function to normalize a word.

`normalizeWord` loops through every character in the string and calls tolower, from ctype.h, to turn every uppercase alphabet character into lowercase

### pagedir

`pagedir.c` was a module created for the Crawler module of the Tiny Search engine.

`pagedir_createPage` was written to aid in `indexBuild`. It encapsulates the code to create a new webpage, given a file created by crawler. Pseudocode:

```
Parse url using file_readWord
Parse the depth
Call webpage_new to create a new webpage with NULL html
Free necessary copies
Return the created page
```

## Functions prototypes

### indexer

Detailed descriptions of each function's interface is provided as a paragraph comment in `indexer.h`. In the case of static functions, the paragraph comment can be found in `indexer.c`

```c
int main(const int argc, char* argv[]); 
static void parseArgs(const int argc, char* argv[], char** pageDirectory, char** indexFilename);
void indexBuild(index_t* idx, const char *pageDirectory); 
static void indexPage (webpage_t* page, int docID, index_t* idx);
```

### indexTest

```c
int main(const int argc, const char *argv[]); 
```


### index

Detailed descriptions of each function's interface is provided as a paragraph comment in `index.h`. In the case of static functions, the paragraph comment can be found in `index.c`

```c
static void counterDeleteHelper(void *ctr); 
static hashtable_t* getIndexHashtable(index_t *idx); 
static void saveHashtable(void* indexFile, const char* word, void* ctr); 
static void saveCounters(void* indexFile, const int docID, const int count);
index_t* index_new(const int num_slots); 
void index_insert(index_t* idx, const char* word, counters_t* ctr); 
void index_increment(index_t* idx, const char* word, const int docID); 
counters_t* index_get(index_t* idx, const char* key); 
void index_delete(index_t* idx); 
void index_saveToFile(index_t* idx, const char* indexFilename); 
index_t* index_load(const char* indexFilename);
```

### word

Detailed descriptions of each function's interface is provided as a paragraph comment in `word.h`

```c
void normalizeWord(char* word); 
```

### pagedir

Detailed descriptions of each function's interface is provided as a paragraph comment in `pagedir.h`. Only the function created for use by indexer are included here. 

```c
webpage_t* pagedir_createPage(FILE* fp); 
```

## Error handling and recovery

All the command-line parameters are rigorously checked before any data structures are allocated or work begins; problems result in a message printed to stderr and a non-zero exit status.

Out-of-memory errors are handled by mem_assert function, which result in a message printed to stderr and a non-zero exit status. We anticipate out-of-memory errors to be rare and thus allow the program to crash (cleanly) in this way.

All code uses defensive-programming tactics to catch and exit, e.g., if a function receives bad parameters.

That said, certain errors are caught and handled internally: for example, many functions will simply return NULL or ignore faulty inputs

## Testing plan

Here is the implementation-specific testing plan

### Unit testing

The modules that are not `indexer.c` and `indexTest.c` were deemed sufficient to observe their behavior during the system test

### Regression testing

For routine regression tests, we compare the current output to the output from a working run. To compare, we use `indexcmp`, whose usage is:

```
~/cs50-dev/shared/tse/indexcmp indexFilenameA indexFilenameB
```

NOTE: Regression tests are not scripted in `testing.sh`, but rather done during development

### Integration/ system testing

We write a script `testing.sh` that invokes the indexer serveral times
1. With a variety of command-line argument. , a sequence of invocations with invalid/ erroneous arguments, testing each possible mistake. 
2. A run of indexer over letters10 without valgrind. 
3. A run of indexer over wikipedia1 with valgrind
4. A run of indexTest over wikipedia1 without valgrind
5. A run of indexTest over letters10 with valgrind

Run that script with `bash -v testing.sh` so that the output of indexer is intermixed with the commands used to invoke the indexer. Verify correct behavior by studying the output. Alternatively, for the runs with valid arguments, correct behavior can be verified by running `indexcmp`. 

To redirect stdout to `testing.out`, simply run `make test`

The output to `testing.sh` can be found in `testing.out`
