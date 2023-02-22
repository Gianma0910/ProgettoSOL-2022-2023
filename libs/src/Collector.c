/**
 * File that contains the implementation of Collector. It is the child process and single-threaded
 * @author Gianmarco Petrocchi
 */

#include "../Collector.h"
#include "../utilities.h"

#define SOCKETNAME "./farm.sck"

/**
 * Flag used to identify if the signal SIGUSR2 occurs.
 */
volatile sig_atomic_t flag_sigusr2 = 0;

static void handler(int signum);

int run_collector(int pid_parent_process){
    bool error_occur = false;
    int fd_socket;
    ascending_queue *ascending_q = NULL;
    ascending_q = create_ascending_queue();

    if(ascending_q == NULL){
        fprintf(stderr, "Error in creating the ascending queue\n");
        shutdown_collector_prematurely(NULL, -1, SOCKETNAME, pid_parent_process);
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
        shutdown_collector_prematurely(ascending_q, -1, SOCKETNAME, pid_parent_process);
        return -1;
    }

    if(sigaction(SIGHUP, &s, NULL) == -1){
        perror("sigaction SIGHUP");
        shutdown_collector_prematurely(ascending_q, -1, SOCKETNAME, pid_parent_process);
        return -1;
    }

    if(sigaction(SIGINT, &s, NULL) == -1){
        perror("sigaction SIGINT");
        shutdown_collector_prematurely(ascending_q, -1, SOCKETNAME, pid_parent_process);
        return -1;
    }

    if(sigaction(SIGTERM, &s, NULL) == -1){
        perror("sigaction SIGTERM");
        shutdown_collector_prematurely(ascending_q, -1, SOCKETNAME, pid_parent_process);
        return -1;
    }

    if(sigaction(SIGQUIT, &s, NULL) == -1){
        perror("sigaction SIGQUIT");
        shutdown_collector_prematurely(ascending_q, -1, SOCKETNAME, pid_parent_process);
        return -1;
    }

    if(sigaction(SIGPIPE, &s, NULL) == -1){
        perror("sigaction SIGPIPE");
        shutdown_collector_prematurely(ascending_q, -1, SOCKETNAME, pid_parent_process);
        return -1;
    }

    s.sa_handler = handler;

    if(sigaction(SIGUSR2, &s, NULL) == -1){
        perror("sigaction SIGUSR2");
        shutdown_collector_prematurely(ascending_q, -1, SOCKETNAME, pid_parent_process);
        return -1;
    }

    if(flag_sigusr2 == 1){
        shutdown_collector(ascending_q, -1, NULL);
        return -1;
    }

    if((fd_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("socket");
        shutdown_collector_prematurely(ascending_q, -1, SOCKETNAME, pid_parent_process);
        return -1;
    }

    if(flag_sigusr2 == 1){
        shutdown_collector(ascending_q, fd_socket, SOCKETNAME);
        return -1;
    }

    while(connect(fd_socket, (struct sockaddr *) &sa, sizeof(sa)) == -1){
        if(flag_sigusr2 == 1){
            shutdown_collector(ascending_q, fd_socket, SOCKETNAME);
            error_occur = true;
            break;
        }else{
            if(errno == ENOENT || errno == ECONNREFUSED)
                sleep(1);
            else{
                shutdown_collector_prematurely(ascending_q, fd_socket, SOCKETNAME, pid_parent_process);
                error_occur = true;
                break;
            }

        }
    }

    if(error_occur == true){
        return -1;
    }

    while(1){
        ssize_t bytes_read;
        int length_string_received;
        int length_filename;
        char* filename;
        char* string_received;
        long result;

        bytes_read = readn(fd_socket, &length_string_received, sizeof(int));
        if(bytes_read != sizeof(int)){
            fprintf(stderr, "Collector: error in readn(), more or less bytes have been read than necessary\n");
            shutdown_collector_prematurely(ascending_q, fd_socket, SOCKETNAME, pid_parent_process);
            error_occur = true;
            break;
        }else{
            string_received = malloc(sizeof(char) * length_string_received);
            bytes_read = readn(fd_socket, string_received, sizeof(char) * length_string_received);
            if(bytes_read != (sizeof(char) * length_string_received)){
                fprintf(stderr, "Collector: error in readn(), more or less bytes have been read than necessary\n");
                free(string_received);
                shutdown_collector_prematurely(ascending_q, fd_socket, SOCKETNAME, pid_parent_process);
                error_occur = true;
                break;
            }else{
                if(strncmp(string_received, "print", strlen("print")) == 0){
                    fprintf(stdout, "SIGUSR1 communication:\n");
                    print_ascending_queue(ascending_q);
                    free(string_received);
                }else if(strncmp(string_received, "finish", strlen("finish")) == 0){
                    free(string_received);
                    break;
                }else{
                    result = strtol(string_received, &filename, 10);
                    bytes_read = readn(fd_socket, &length_filename, sizeof(int));
                    if(bytes_read != sizeof(int)){
                        fprintf(stderr, "Collector: error in readn(), more or less bytes have been read than necessary\n");
                        shutdown_collector_prematurely(ascending_q, fd_socket, SOCKETNAME, pid_parent_process);
                        error_occur = true;
                        break;
                    }else{
                        push_ascending(&ascending_q, filename, result, length_filename);
                    }
                    free(string_received);
                }
            }
        }
    }

    if(error_occur == true){
        return -1;
    }else{
        fprintf(stdout, "Final result\n");
        print_ascending_queue(ascending_q);
        free_ascending_queue(ascending_q);
        close(fd_socket);
    }

    return 0;
}

void shutdown_collector(ascending_queue* q, int socket, char* socket_name){
    if(q != NULL){
        free(q);
    }

    if(socket != -1){
        close(socket);
    }

    if(socket_name != NULL){
        remove(socket_name);
    }

    return;
}

void shutdown_collector_prematurely(ascending_queue* q, int socket, char* socket_name, int pid_parent_process){
    if(q != NULL){
        free_ascending_queue(q);
    }

    if(socket != -1){
        close(socket);
    }

    if(socket_name != NULL){
        remove(socket_name);
    }

    kill(pid_parent_process, SIGUSR2);

    return;
}

/**
 * Method used to handle signal SIGUSR2
 * @param signum Integer that identifies the signal occurs
 */
static void handler(int signum){
    if(signum == SIGUSR2){
        flag_sigusr2 = 1;
    }
}


