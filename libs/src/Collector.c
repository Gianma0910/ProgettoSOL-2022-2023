/**
 * File that contains the implementation of Collector. It is the child process and single-threaded
 * @author Gianmarco Petrocchi
 */

#include "../Collector.h"
#include "../utilities.h"

#define SOCKETNAME "./farm.sck"

int run_collector(){
    int fd_socket;
    ascending_queue *ascending_q = NULL;
    ascending_q = create_ascending_queue();

    if(ascending_q == NULL){
        fprintf(stderr, "Error in creating the ascending queue\n");
        shutdown_collector(NULL, -1, SOCKETNAME);
        return -1;
    }

    struct sockaddr_un sa;
    strcpy(sa.sun_path, SOCKETNAME);
    sa.sun_family = AF_UNIX;

    struct sigaction s;
    memset(&s, 0, sizeof(s));
    s.sa_handler = SIG_IGN;

    if(sigaction(SIGUSR1, &s, NULL) == -1){
        perror("sigaction SIGUSR1");
        shutdown_collector(ascending_q, -1, SOCKETNAME);
        return -1;
    }

    if(sigaction(SIGHUP, &s, NULL) == -1){
        perror("sigaction SIGHUP");
        shutdown_collector(ascending_q, -1, SOCKETNAME);
        return -1;
    }

    if(sigaction(SIGINT, &s, NULL) == -1){
        perror("sigaction SIGINT");
        shutdown_collector(ascending_q, -1, SOCKETNAME);
        return -1;
    }

    if(sigaction(SIGTERM, &s, NULL) == -1){
        perror("sigaction SIGTERM");
        shutdown_collector(ascending_q, -1, SOCKETNAME);
        return -1;
    }

    if(sigaction(SIGQUIT, &s, NULL) == -1){
        perror("sigaction SIGQUIT");
        shutdown_collector(ascending_q, -1, SOCKETNAME);
        return -1;
    }

    if(sigaction(SIGPIPE, &s, NULL) == -1){
        perror("sigaction SIGPIPE");
        shutdown_collector(ascending_q, -1, SOCKETNAME);
        return -1;
    }

    if((fd_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("socket");
        shutdown_collector(ascending_q, -1, SOCKETNAME);
        return -1;
    }

    while(connect(fd_socket, (struct sockaddr *) &sa, sizeof(sa)) == -1){
        if(errno == ENOENT || errno == ECONNREFUSED || errno == EIO || errno == EPIPE)
                sleep(1);
        else{
            shutdown_collector(ascending_q, fd_socket, SOCKETNAME);
            return -1;
        }
    }

    while(1){
        int length_filename;
        long result;
        char* filename = NULL;
        ssize_t bytes_read;

        bytes_read = readn(fd_socket, &length_filename, sizeof(int));
        if(bytes_read == -1){
            perror("readn");
            shutdown_collector(ascending_q, fd_socket, SOCKETNAME);
            return -1;
        }else if(bytes_read == 0){
            fprintf(stderr, "Socket server side closed, first readn\n");
            shutdown_collector(ascending_q, fd_socket, SOCKETNAME);
            return -1;
        }

        filename = malloc(sizeof(char) * length_filename);
        bytes_read = readn(fd_socket, filename, (sizeof(char) * length_filename));
        if(bytes_read == -1){
            perror("readn");
            free(filename);
            shutdown_collector(ascending_q, fd_socket, SOCKETNAME);
            return -1;
        }else if(bytes_read == 0){
            fprintf(stderr, "Socket server side closed, second readn\n");
            shutdown_collector(ascending_q, fd_socket, SOCKETNAME);
            return -1;
        }

        if(strncmp(filename, "print", strlen("print")) == 0){
            fprintf(stdout, "SIGUSR1 communication:\n");
            print_ascending_queue(ascending_q);
            free(filename);
        }else if(strncmp(filename, "finish", strlen("finish")) == 0){
            free(filename);
            break;
        }else{
            if(readn(fd_socket, &result, sizeof(unsigned long)) == -1){
                perror("readn");
                shutdown_collector(ascending_q, fd_socket, SOCKETNAME);
                free(filename);
                return -1;
            }else{
                push_ascending(&ascending_q, filename, result, length_filename);
                free(filename);
            }
        }
    }

    print_ascending_queue(ascending_q);
    free_ascending_queue(ascending_q);
    close(fd_socket);

    return 0;
}

void shutdown_collector(ascending_queue* q, int socket, char* socket_name){
    if(q != NULL){
        free_ascending_queue(q);
    }

    if(socket != -1){
        close(socket);
    }

    if(socket_name != NULL){
        remove(socket_name);
    }

    return;
}