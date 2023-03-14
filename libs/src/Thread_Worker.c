/**
 * File that contains the implementation of thread worker
 * @author Gianmarco Petrocchi
 */

#include "../Thread_Worker.h"

void* run_thread(void* arg){
    thread_parameter* param = (thread_parameter *) arg;

    while(1){

        if((*param->flag_print) > 0){
            char* string = "print";
            int length = (int) strlen(string);
            pthread_mutex_lock(&(param->mutex_socket));
            if(writen(param->socket, &length, sizeof(int)) == -1){
                perror("writen");
                pthread_exit(0);
            }else{
                if(writen(param->socket, string, sizeof(char) * length) == -1){
                    perror("writen");
                    pthread_exit(0);
                }else{
                    (*param->flag_print)--;
                }
            }
            pthread_mutex_unlock(&(param->mutex_socket));
        }else if((*param->flag_others_signals) == 1){
            data* d = pop(param->queue);

            if(d == NULL){
                break;
            }else if(strncmp(d->filename, "finish", strlen("finish")) == 0){
                free(d->filename);
                free(d);
                pthread_exit(0);
            }else{
                file_calculation(d, param);
            }
        }else{
            data* d = pop(param->queue);

            if(d == NULL){
                break;
            }else if(strncmp(d->filename, "finish", strlen("finish")) == 0){
                free(d->filename);
                free(d);
                pthread_exit(0);
            }else{
                file_calculation(d, param);
            }
        }
    }

    return (void *) 0;
}

long sum_elem_buffer(const long buffer[], int length_buffer){
    long result = 0;

    for(int i = 0; i < length_buffer; i++){
        result = result + (i * buffer[i]);
    }

    return result;
}

void file_calculation(data* d, thread_parameter* param){
    ssize_t bytes_written;

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

    fclose(file);
    free(buffer);

    pthread_mutex_lock(&(param->mutex_socket));
    bytes_written = writen(param->socket, &(d->length_str_filename), sizeof(int));
    if(bytes_written == -1 || bytes_written == 0){
        free(d->filename);
        free(d);
        pthread_mutex_unlock(&(param->mutex_socket));
        pthread_exit(0);
    }

    bytes_written = writen(param->socket, d->filename, (sizeof(char) * d->length_str_filename));
    if(bytes_written == -1 || bytes_written == 0){
        free(d->filename);
        free(d);
        pthread_mutex_unlock(&(param->mutex_socket));
        pthread_exit(0);
    }

    bytes_written = writen(param->socket, &result, sizeof(unsigned long));
    if(bytes_written == -1 || bytes_written == 0){
        free(d->filename);
        free(d);
        pthread_mutex_unlock(&(param->mutex_socket));
        pthread_exit(0);
    }
    pthread_mutex_unlock(&(param->mutex_socket));

    free(d->filename);
    free(d);
}