/**
 * File that contains the implementation of thread worker
 * @author Gianmarco Petrocchi
 */

#include "../Thread_Worker.h"

void* run_thread(void* arg){
    thread_parameter* param = (thread_parameter *) arg;
    ssize_t bytes_written;

    while(1){

        if((*param->flag_print) > 0){
            char* string = "print";
            int length = (int) strlen(string);
            bytes_written = writen(param->socket, &length, sizeof(int));
            if(bytes_written != sizeof(int)){
                fprintf(stderr, "Thread worker: error in writen(), more or less bytes have been written than necessary\n");
            }else{
                bytes_written = writen(param->socket, string, sizeof(char) * length);
                if(bytes_written != (sizeof(char) * length)){
                    fprintf(stderr, "Thread worker: error in writen(), more or less bytes have been written than necessary\n");
                }else{
                    (*param->flag_print)--;
                }
            }
        }else if((*param->flag_others_signals) == 1){
            data* d = pop(param->queue);

            if(d == NULL){
                break;
            }else if(strncmp(d->filename, "finish", strlen("finish")) == 0){
                free(d->filename);
                free(d);
                break;
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
                break;
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
    char* result_to_string = malloc(sizeof(char) + sizeof(result) + sizeof(long));
    sprintf(result_to_string, "%ld", result);

    fclose(file);
    free(buffer);

    char* string_to_send = str_concat(result_to_string, d->filename);
    int length_string_to_send = (int) strlen(string_to_send);

    bytes_written = writen(param->socket, &length_string_to_send, sizeof(int));
    if(bytes_written != sizeof(int)){
        fprintf(stderr, "Thread worker: error in writen(), more or less bytes have been written than necessary\n");
        free(result_to_string);
        free(d->filename);
        free(d);
        free(string_to_send);
        return;
    }else{
        bytes_written = writen(param->socket, string_to_send, sizeof(char) * length_string_to_send);
        if(bytes_written != (sizeof(char) * length_string_to_send)){
            fprintf(stderr, "Thread worker: error in writen(), more or less bytes have been written than necessary\n");
            free(result_to_string);
            free(d->filename);
            free(d);
            free(string_to_send);
            return;
        }else{
            bytes_written = writen(param->socket, &(d->length_str_filename), sizeof(int));
            if(bytes_written != sizeof(int)){
                fprintf(stderr, "Thread worker: error in writen(), more or less bytes have been written than necessary\n");
                free(result_to_string);
                free(d->filename);
                free(d);
                free(string_to_send);
                return;
            }
        }
    }

    free(result_to_string);
    free(d->filename);
    free(d);
    free(string_to_send);
}