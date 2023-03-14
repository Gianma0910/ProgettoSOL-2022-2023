/**
 * File header that contains only the declaration of the CLI parameters with its default value
 * @author Gianmarco Petrocchi
 */

#ifndef PROGETTOSOL_PARAMETERS_H
#define PROGETTOSOL_PARAMETERS_H

#include <stddef.h>

/**
 * Number of thread workers inside the pool. Default value is 4
 */
int num_thread_worker = 4;
/**
 * Length of the queue. Default value is 8
 */
int queue_length = 8;
/**
 * Name of the directory to scan. Default value is NULL
 */
char* directory_name = NULL;
/**
 * Delay between two insertion inside the queue. Default value is 0
 */
int time_delay = 0;

#endif //PROGETTOSOL_PARAMETERS_H
