/* 
 * crawler.h - header file for TSE crawler module
 *
 * crawler "crawls" webpages and saves files containing information used by
 *  querieir and saves HTML from those webpages
 * 
 * usage: crawler seedURL pageDirectory maxDepth
 *
 * Andres Ibarra, February 2022
 */

#include "hashtable.h"
#include "bag.h"
#include "webpage.h"
#include "pagedir.h"

/**************** parseArgs ****************/
/* 
 * Validate and unpack command-line args
 * We guarantee:
 *  URL will be initialized
 *  pageDirectory is readable/ writeable
 *  maxDepth is within a reasonable range
 * Note: 
 *  Exit non-zero if error encountered
 */
static void parseArgs(const int argc, char* argv[],
                      char** seedURL, char** pageDirectory, int* maxDepth);

/**************** crawl ****************/
/* 
 * "Crawls" webpages starting at seedURL until maxDepth is reached, 
 * 
 * Caller provides
 *  Valid char* representing the seedURL, already normalized and validated
 *    as an internal URL
 *  Valid char* representing pageDirectory, already validated
 *  Valid integer represented max depth to crawl to
 * We do: 
 *  call pagedir_save to create files in pageDirectory containing information
 *    about the webpage later used in other modules
 * We guarantee:
 *  Files will be created in the provided pageDirectory, one per webpage
 *    crawled, named using unique document ID, containing information
 *    about the relevant webpage
 * Note: 
 *  Exit nonzero if error while fetching web page
 */
static void crawl(char* seedURL, char* pageDirectory, const int maxDepth);

/**************** pageScan ****************/
/*
 * Scans page for embedded URL's. Adds them to bag if not already found in 
 *  the hashtable 
 * 
 * Caller provides: 
 *  A valid webpage page, an initialized bag pagesToCrawl, and an initialized
 *    hashtable pagesSeen
 * We do: 
 *  Insert all internal URL's found in the webpage (not already found in 
 *    hashtable pagesSeen() into pagesToCrawl
 * We guarantee:
 *  External and duplicate URLS will not be added to the bag
 */
static void pageScan(webpage_t* page, bag_t* pagesToCrawl, hashtable_t* pagesSeen);