#ifndef PROGETTOSOL_THREAD_WORKER_H
#define PROGETTOSOL_THREAD_WORKER_H

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include "utilities.h"
#include "queue.h"
#include "mystring.h"

/**
 * Struct that represents all the parameters needed by a thread
 * @param socket File descriptor of the socket per communicate with Collector
 * @param queue Queue where thread pop the elements
 * @param flag_print Flag to control if signal SIGUSR1 occurs
 * @param flag_others_signals Flag to control if one of those signals occurs: SIGINT, SIGHUP, SIGTERM, SIGQUIT
 */
typedef struct thread_parameter{
    int socket;
    queue** queue;
    volatile sig_atomic_t* flag_print;
    volatile sig_atomic_t* flag_others_signals;
    pthread_mutex_t mutex_socket;
}thread_parameter;

/**
 * Method used to start the thread's run
 * @param arg Argument passed to thread, it must be a pointer to struct thread_parameter
 * @return
 */
void* run_thread(void* arg);
/**
 * Method used to sum elements of a long buffer
 * @param buffer Buffer of long number to sum
 * @param length_buffer Length of the buffer
 * @return The result of the sum
 */
long sum_elem_buffer(const long buffer[], int length_buffer);
/**
 * Method used to open and the file inside the struct data. This method uses the sum_elem_buffer method to calculate the result
 * @param d Pointer to a struct data
 * @param param Parameters of the thread worker
 */
void file_calculation(data* d, thread_parameter* param);

#endif //PROGETTOSOL_THREAD_WORKER_H
