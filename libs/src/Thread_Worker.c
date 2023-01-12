/**
 * File that contains the implementation of thread worker
 * @author Gianmarco Petrocchi
 */

#include "../Thread_Worker.h"

/**
 * Mutex used to make the access to socket synchronized
 */
pthread_mutex_t mutex_socket = PTHREAD_MUTEX_INITIALIZER;
/**
 * Mutex used to make the modification of flag_finish synchronized
 */
pthread_mutex_t mutex_flag = PTHREAD_MUTEX_INITIALIZER;
/**
 * Used to set the end of the threads' run
 */
static bool flag_finish = false;

void* run_thread(void* arg){
    thread_parameter* param = (thread_parameter *) arg;

    while(1){
        if(flag_finish == true) {
            break;
        }

        if((*param->flag_print) == 1){
            pthread_mutex_lock(&mutex_socket);
            char* string = "print";
            int length = (int) strlen(string);
            writen(param->socket, &length, sizeof(int));
            writen(param->socket, string, sizeof(char) * length);
            *(param->flag_print) = 0;
            pthread_mutex_unlock(&mutex_socket);
        }else if((*param->flag_others_signals) == 1){
            while(1){
                data* d = pop(param->queue);

                if(d == NULL){
                    break;
                }else if(strncmp(d->filename, "finish", strlen("finish")) == 0){
                    shutdown_thread_worker(d, param);
                    break;
                }else{
                    file_calculation(d, param);
                }
            }
            break;
        }else{
            data* d = pop(param->queue);

            if(d == NULL){
                break;
            }else if(strncmp(d->filename, "finish", strlen("finish")) == 0){
                shutdown_thread_worker(d, param);
                break;
            }else{
                file_calculation(d, param);
            }
        }
    }

    return (void *) 0;
}

long sum_elem_buffer(long buffer[], int length_buffer){
    long result = 0;

    for(int i = 0; i < length_buffer; i++){
        result = result + (i * buffer[i]);
    }

    return result;
}

void file_calculation(data* d, thread_parameter* param){

    FILE* file = fopen(d->filename, "rb");

    if(file == NULL){
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    if((fseek(file, 0L, SEEK_END)) != 0){
        perror("fseek");
        exit(EXIT_FAILURE);
    }

    int file_size = (int) ftell(file);
    int number_of_numbers_in_file = file_size/8;
    rewind(file);

    long* buffer = malloc(sizeof(long) * number_of_numbers_in_file);

    fread(buffer, sizeof(long), number_of_numbers_in_file, file);

    long result = sum_elem_buffer(buffer, number_of_numbers_in_file);
    char* result_to_string = malloc(sizeof(char) + sizeof(result) + sizeof(long));
    sprintf(result_to_string, "%ld", result);

    fclose(file);
    free(buffer);

    pthread_mutex_lock(&mutex_socket);
    char* string_to_send = str_concat(result_to_string, d->filename);
    int length_string_to_send = (int) strlen(string_to_send);

    writen(param->socket, &length_string_to_send, sizeof(int));
    writen(param->socket, string_to_send, sizeof(char) * length_string_to_send);
    writen(param->socket, &(d->length_str_filename), sizeof(int));

    free(result_to_string);
    free(d->filename);
    free(d);
    free(string_to_send);
    pthread_mutex_unlock(&mutex_socket);
}

void shutdown_thread_worker(data* d, thread_parameter* param){
    pthread_mutex_lock(&mutex_flag);
    flag_finish = true;
    pthread_mutex_unlock(&mutex_flag);

    pthread_mutex_lock(&mutex_socket);
    int length = (int) strlen(d->filename);
    writen(param->socket, &length, sizeof(int));
    writen(param->socket, d->filename, sizeof(char) * length);

    free(d->filename);
    free(d);
    pthread_mutex_unlock(&mutex_socket);
}