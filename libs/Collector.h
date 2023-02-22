#ifndef PROGETTOSOL_COLLECTOR_H
#define PROGETTOSOL_COLLECTOR_H

#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <signal.h>
#include "ascending_queue.h"

/**
 * Method used to run the Collector
 * @param socket_name Name of the socket used to communicate with MasterWorker
 * @return An integer: 0 if the Collector finishes with success, -1 otherwise
 */
int run_collector(int pid_parent_process);
/**
 * Method used to shutdown the Collector and to free the memory allocated.
 * It is used only in case the signal SIGUSR2 is detected.
 * @param q Possible queue to free, if it's set NULL it can't be free
 * @param socket Possible socket to free, if it's set -1 it can't be free
 * @param socket_name Possible name of socket to remove, it if's set NULL it can't be free
 */
void shutdown_collector(ascending_queue* q, int socket, char* socket_name);
/**
 * Method used to shutdown the Collector when an error occurs. It also free the memory allocated.
 * @param q Possible queue to free, if it's set NULL it can't be free
 * @param socket Possible socket to free, if it's set -1 it can't be free
 * @param socket_name Possible name of socket to remove, if it's set NULL it can't be free
 * @param pid_parent_process Possible pid of parent process to send SIGUSR2 signal, it it's set -1 is impossible to send the signal
 */
void shutdown_collector_prematurely(ascending_queue* q, int socket, char* socket_name, int pid_parent_process);


#endif //PROGETTOSOL_COLLECTOR_H
