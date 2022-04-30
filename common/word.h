/* 
 * index.h - header file for word module for TSE
 *
 * a module providing a function to normalize a word.
 *
 * Andres Ibarra, February 2022
 */

/**************** normalizeWord() ****************/
/*
 * Uses tolower() in ctype.h to make word all lowercase
 * Caller provides:
 *   Null terminated string word
 * We guarantee:
 *   If not already lowercase, all alpha character in string will be made so
 */
void normalizeWord(char* word);
