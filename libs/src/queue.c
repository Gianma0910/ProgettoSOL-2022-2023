/**
 * File that contains the implementation of queue. It is used by MasterWorker (producer) and thread worker (consumer)
 */

#include "../queue.h"

/**
 * Mutex used to make the queue synchronized
 */
pthread_mutex_t mutex_queue = PTHREAD_MUTEX_INITIALIZER;
/**
 * Used to put threads workers on wait status when the queue is empty
 */
pthread_cond_t empty_queue = PTHREAD_COND_INITIALIZER;
/**
 * Used to put the MasterWorker on wait status when the queue is full
 */
pthread_cond_t full_queue = PTHREAD_COND_INITIALIZER;

queue* create_queue(int size){
    queue* q = malloc(sizeof(queue));

    q->head = NULL;
    q->tail = NULL;
    q->size = size;

    return q;
}

bool queue_isEmpty(queue* q){
    return q->head == NULL;
}

bool queue_isFull(queue* q){
    return q->size == 0;
}

void push(queue** q, char* filename, int length_str_filename){
    if(filename == NULL || q == NULL) return;

    pthread_mutex_lock(&mutex_queue);

    while(queue_isFull((*q)) == true){
        pthread_cond_wait(&full_queue, &mutex_queue);
    }

    node* new_node = malloc(sizeof(node));

    new_node->filename = strndup(filename, strlen(filename));
    new_node->length_str_filename = length_str_filename;
    new_node->next = NULL;

    if((*q)->head == NULL){
        (*q)->head = new_node;
        (*q)->tail = new_node;

        (*q)->size--;
    }else{
        (*q)->tail->next = new_node;
        (*q)->tail = new_node;

        (*q)->size--;
    }

    pthread_cond_signal(&empty_queue);
    pthread_mutex_unlock(&mutex_queue);
}

data* pop(queue** q){
    if(q == NULL) return NULL;

    pthread_mutex_lock(&mutex_queue);

    while(queue_isEmpty((*q)) == true){
        pthread_cond_wait(&empty_queue, &mutex_queue);
    }

    node* temp = (*q)->head;

    data* d = malloc(sizeof(data));
    d->filename = temp->filename;
    d->length_str_filename = temp->length_str_filename;

    (*q)->head = (*q)->head->next;

    free(temp);

    if((*q)->head == NULL){
        (*q)->tail = NULL;
    }
    (*q)->size++;

    pthread_cond_signal(&full_queue);
    pthread_mutex_unlock(&mutex_queue);

    return d;
}

void free_queue(queue* q){
    pthread_mutex_lock(&mutex_queue);

    if(queue_isEmpty(q)){
        free(q);
    }else{
        while(q->head->next != NULL){
            node* temp = q->head;
            q->head = temp->next;
            free(temp->filename);
            temp->next = NULL;

            free(temp);
            q->size++;
        }

        free(q->head->filename);
        q->head->next = NULL;
        free(q->head);
        free(q);
    }
    
    pthread_cond_broadcast(&empty_queue);
    pthread_mutex_unlock(&mutex_queue);
}