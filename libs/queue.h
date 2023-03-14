#ifndef PROGETTOSOL_QUEUE_H
#define PROGETTOSOL_QUEUE_H

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <pthread.h>

/**
 * Struct that represents an element of the queue
 * @param filename String that represents the name of the file
 * @param length_str_filename Integer that represents the length of filename
 * @param next Pointer to the next element of the queue
 */
typedef struct node{
    char* filename;
    int length_str_filename;
    struct node* next;
}node;

/**
 * Struct that represents the queue
 * @param head Pointer to the first element of the queue
 * @param tail Pointer to the last element of the queue
 * @param size Integer that represents the total length of the queue
 */
typedef struct queue{
    node* head;
    node* tail;
    int size;
}queue;

typedef struct data{
    char* filename;
    int length_str_filename;
}data;

/**
 * Method used to create a queue of the given size
 * @param size Total length of the queue
 * @return Queue of the given size
 */
queue* create_queue(int size);
/**
 * Method used to insert an element into the queue. The size of the queue will increase
 * @param q Queue where to insert the element
 * @param filename Name of the file
 * @param length_str_filename Length of filename
 */
void push(queue** q, char* filename, int length_str_filename);
/**
 * Method used to pop the head of the queue. The size of the queue will decrease
 * @param q Queue where to pop the head
 * @return A pointer to the struct data (filename, length_str_filename)
 */
data* pop(queue** q);
/**
 * Method used to free/destroy the given queue
 * @param q Queue to free/destroy
 */
void free_queue(queue* q);
/**
 * Check if the queue is empty
 * @param q Queue to query
 * @return A boolean value: true if the queue is empty, false otherwise
 */
bool queue_isEmpty(queue* q);
/**
 * Check if the queue is full
 * @param q Queue to query
 * @return A boolean value: true if the queue is empty, false otherwise
 */
bool queue_isFull(queue* q);

#endif //PROGETTOSOL_QUEUE_H
