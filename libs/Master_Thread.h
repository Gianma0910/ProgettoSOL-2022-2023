#ifndef PROGETTOSOL_MASTER_THREAD_H
#define PROGETTOSOL_MASTER_THREAD_H

#include <sys/wait.h>
#include <dirent.h>
#include <pthread.h>
#include <sys/un.h>
#include <stddef.h>
#include <sys/socket.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include "queue.h"
#include "Thread_Worker.h"
#include "utilities.h"
#include "mystring.h"
/**
 * Method used to run the MasterWorker
 * @param number_parameters Number of CLI parameters passed to main
 * @param parameters Array of strings that represents the CLI parameter passed to main.
 * This parameters are used to configure the execution environment (queue, thread worker)
 * @param child_process Identifier of the child process (COllector)
 * @param socket_name Name of the socket used to communicate with the Collector
 * @return An integer: 0 if it finishes with success, -1 otherwise
 */
int run_master_thread(int number_parameters, char* parameters[], int child_process);
/**
 * Method used to create thread worker of the thread pool
 * @param n_threads Number of thread worker to create
 * @param param Parameter of the thread worker
 * @return An array of thread worker created
 */
pthread_t* spawn_threads(int n_threads, thread_parameter* param);
/**
 * Method used to scan the given directory. If a legit file is found inside the directory, it will be insert into the queue,
 * otherwise if it is a sub directory it will be scan.
 * If during the scan will be detect the following signals: SIGINT, SIGQUIT, SIGTERM, SIGHUP, it will be call the method
 * shutdown_master_thread
 * @param dirname Name of the directory to scan
 * @param q Queue where to insert the file
 * @param thread_worker Array of thread worker that can be free, only if occurs one of the four signals
 * @param child_process Identifier of the child process that the MasterWorker must wait, only if occurs one of the four signals
 * @param fd_connected Integer that represents the file descriptor of socket created with accept that can be close, only if occurs one of the four signal
 * @param fd_socket Integer that represents the file descriptor of socket that can be close, only if occurs one of the four signals
 * @param socket_name Name of the socket that can be remove, only if occurs one of the four signals
 * @param param Parameters used by thread worker that can be free, only if occurs one of the four signals
 */
void scan_dir(char* dirname, queue* q, pthread_t* thread_worker, int child_process, int fd_connected, int fd_socket, char* socket_name, thread_parameter* param);
/**
 * * Method used to shutdown the MasterThread and to free the memory allocated
 * @param thread_worker Array of the thread worker that will be free
 * @param child_process Identifier of the child process that MasterWorker must wait
 * @param fd_socket Integer that represents the file descriptor of socket that will be close
 * @param socket_name Name of the socket that will be remove
 * @param param Parameters used by thread worker that will be free
 */
void shutdown_master_thread(pthread_t* thread_worker, int child_process, int fd_socket, char* socket_name, thread_parameter* param);
/**
 * Method used to shutdown prematurely the MasterThread and free the memory allocated. It is used only in case if a syscall had an error,
 * if there was an error during the configuration of CLI parameters, if there was an error while writing into the socket
 * @param child_process Identifier of the child process that MasterWorker must wait and must send the SIGUSR2 signal
 * @param fd_connected Integer that represents the file descriptor of socket created with accept that will be close
 * @param fd_socket Integer that represents the file descriptor of socket that will be close
 * @param socket_name Name of the socket that will be remove
 * @param q Queue that will be free/close
 */
void shutdown_master_thread_prematurely(int fd_connected, int fd_socket, char* socket_name, queue* q);

#endif //PROGETTOSOL_MASTER_THREAD_H
