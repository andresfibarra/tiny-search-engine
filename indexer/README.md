# Andres Ibarra
## CS50 Spring 2021, Lab 5 TSE Indexer

GitHub username: @andresfibarra

### Instructions

To build, run `make` in terminal

To clean up make-derived files, run `make clean`

To run testing script with the output redirected to file testing.out, run `make test`

To run testing script with stdout as the output, run `testing.sh` on a Dartmouth linux machine

### Usage

#### Indexer

indexer pageDirectory indexFilename

if `pageDirectory` is the path to a crawler-created directory and `indexFilename` is the path to a file to which the indexer should print the index

#### IndexTest

indexTest oldIndexFilename newIndexFilename 

if `oldIndexFilename` is the path to a file created by indexer, and `newIndexFilename` is the path to a file to which indexTest will print out its copy of the index after parsing

### Assumptions

No assumptions other than those specified in the requirements