/* 
 * pagedir.h - header file for TSE pagedir module
 *
 * Module to handle writing a file to the pageDirectory 
 * and marking it as a Crawler-produced pagedirector
 * 
 * Andres Ibarra, February 2022
 */

#include <stdbool.h>
#include <math.h>
#include "webpage.h"

/**************** pagedir_init ****************/
/* 
 * Initialize pageDirectory and verify that it is valid and that files 
 *  can be created in it
 * 
 * We do:
 *  Create and open a dummy file called ".crawler" in pageDirectory
 * We return: 
 *  true if successful opening of file for writing, false if not
 */
bool pagedir_init(const char* pageDirectory);

/**************** pagedir_save ****************/
/* 
 * Saves the information about webpage_t page in pageDirectory
 * 
 * Caller provides: 
 *  A valid webpage_t page
 *  A valid and initialized (such as from pagedir_init) pageDirectory
 *  A valid docID
 * We do: 
 *  Create a file in pageDirectory with the docID as its name
 *  Save the URL, depth, and HTML of page to that file
 */
void pagedir_save(const webpage_t* page, const char* pageDirectory, const int docID);

/**************** pagedir_createPage ****************/
/* 
 * Helper function to load webpage from file in pageDirectory
 * 
 * Caller provides: 
 *  A readable file in the same format as those created by crawler
 * Caller is responsible for: 
 *  Later calling webpage_delete() on the returned webpage
 *  Calling webpage_fetch() if html is needed
 * We return: 
 *  a webpage_t object with url and docID as found in file
 */
webpage_t* pagedir_createPage(FILE* fp); 
