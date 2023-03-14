#ifndef PROGETTOSOL_ASCENDING_QUEUE_H
#define PROGETTOSOL_ASCENDING_QUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

/**
 * Struct that represents an element of the queue
 * @param filename String that represents the name of a file
 * @param value Long that represents the result calculated from the file
 * @param next Pointer to the next element of the queue
 */
typedef struct result_file{
    char* filename;
    long value;
    struct result_file* next;
}result_file;

/**
 * Struct that represents the entire queue
 * @param head Pointer to the first element of the queue
 * @param tail Pointer to the last element of the queue
 */
typedef struct ascending_queue{
    result_file* head;
    result_file* tail;
}ascending_queue;

/**
 * Method used to create the ascending queue
 * @return Ascending queue created
 */
ascending_queue* create_ascending_queue();
/**
 * Method used to insert an element into the queue
 * @param queue Queue where to insert the element
 * @param filename Name of the file
 * @param value Result calculated from the file
 * @param length_str_filename Length of the string that represents the name of file
 */
void push_ascending(ascending_queue** queue, char* filename, long value, size_t length_str_filename);
/**
 * Method used to print the entire queue
 * @param queue Queue to print
 */
void print_ascending_queue(ascending_queue* queue);
/**
 * Method used to free the queue
 * @param queue Queue to free
 */
void free_ascending_queue(ascending_queue* queue);

#endif //PROGETTOSOL_ASCENDING_QUEUE_H
