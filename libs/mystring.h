#ifndef PROGETTOSOL_MYSTRING_H
#define PROGETTOSOL_MYSTRING_H

#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/**
 * It does the strdup of the given string if it's not empty or NULL
 * @param s String to duplicate
 * @return A new string that is the duplicate of the given
 */
char* str_create(char* s);
/**
 * Create a string that is the concatenation of the two given strings
 * @param s1 First string (destination)
 * @param s2 Second string (source)
 * @return A string that is the result of the concatenation of the two given strings
 */
char* str_concat(char* s1, char* s2);
/**
 * Create a string that is the concatenation of the n given strings
 * @param s1 First string (destination)
 * @param ... Others strings to concatenate
 * @return A string that is the result of the concatenation of the n given strings
 */
char* str_concatn(char* s1, ...);

#endif //PROGETTOSOL_MYSTRING_H
