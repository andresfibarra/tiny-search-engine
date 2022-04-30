/* 
 * word.c - word module for TSE
 *
 * see word.h for more information.
 *
 * Andres Ibarra, February 2022
 */

#include <ctype.h>

/**************** normalizeWord() ****************/
/* see word.h for description */
void normalizeWord(char* word)
{
  for (int i = 0; word[i]; i++) {
    word[i] = tolower(word[i]);
  }
}