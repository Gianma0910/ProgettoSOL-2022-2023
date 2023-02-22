#ifndef PROGETTOSOL_UTILITIES_H
#define PROGETTOSOL_UTILITIES_H

#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stddef.h>
#include <errno.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/**
 * Check if the given string is a number
 * @param s String to check
 * @return The string converted in number if it is, -1 otherwise
 */
long isNumber(const char* s);
/**
 * Check if the given filename is a directory
 * @param filename Name of the file to check
 * @return An integer: 0 if it is a directory, -1 otherwise
 */
int isDir(const char* filename);
/**
 * Check if the given filename is a regular file
 * @param filename Name of the file to check
 * @return An integer: 0 if it is a regular file, -1 otherwise
 */
int isFile(const char* filename);
/**
 * Method used to read n bytes from the socket
 * @param fd File descriptor of the socket where to read
 * @param ptr Pointer to where store the data read
 * @param n Number of bytes to read
 * @return The number of bytes that has been reading
 */
ssize_t readn(int fd, void* ptr, size_t n);
/**
 * Method used to write n bytes from the socket
 * @param fd File descriptor of the socket where to write
 * @param ptr Pointer to the content to write
 * @param n Number of bytes to write
 * @return The number of bytes that has been writing
 */
ssize_t writen(int fd, void* ptr, size_t n);
/**
 * Method used to sleep a thread. It will sleep in milliseconds
 * @param msec Total of milliseconds to sleep
 * @return An integer: 0 success, -1 otherwise
 */
int msleep(long msec);
#endif //PROGETTOSOL_UTILITIES_H
