/**
 * File that contains the implementation of the ascending queue
 * @author Gianmarco Petrocchi
 */

#include "../ascending_queue.h"

ascending_queue* create_ascending_queue(){
    ascending_queue* q = NULL;

    q = malloc(sizeof(ascending_queue));

    q->head = NULL;
    q->tail = NULL;

    return q;
}

void push_ascending(ascending_queue** q, char* filename, long value, size_t length_str_filename){
    result_file* new_node = malloc(sizeof(result_file));

    new_node->filename = strndup(filename, length_str_filename);
    new_node->value = value;
    new_node->next = NULL;

    if((*q)->head == NULL){
        (*q)->head = new_node;
        (*q)->tail = new_node;
    }else{
        result_file* current = (*q)->head;
        result_file* previous = NULL;

        while(current->value <= new_node->value && current->next != NULL){
            previous = current;
            current = current->next;
        }
        if(previous == NULL && current->value > new_node->value){
            new_node->next = current;
            (*q)->head = new_node;
        }else if(current->next == NULL && current->value <= new_node->value){
            current->next = new_node;
            (*q)->tail = new_node;
        }else if((current->next == NULL && current->value > new_node->value) || (current->next != NULL && previous != NULL)){
            previous->next = new_node;
            new_node->next = current;
        }
    }

}

void print_ascending_queue(ascending_queue* q){
    if(q == NULL || (q->head == NULL && q->tail == NULL)){
        return;
    }

    result_file* temp = q->head;

    while(temp->next != NULL){
        fprintf(stdout,"%ld %s\n", temp->value, temp->filename);

        temp = temp->next;
    }

    fprintf(stdout, "%ld %s\n", temp->value, temp->filename);
    fprintf(stdout, "\n");
}

void free_ascending_queue(ascending_queue* q){
    if(q == NULL){
        return;
    }else if(q->head == NULL && q->tail == NULL){
        free(q);
        return;
    }

    while(q->head->next != NULL){
        result_file* temp = q->head;
        q->head = temp->next;
        free(temp->filename);
        temp->next = NULL;

        free(temp);
    }

    free(q->head->filename);
    q->head->next = NULL;
    free(q->head);
    free(q);
}