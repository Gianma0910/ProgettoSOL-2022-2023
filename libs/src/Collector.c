/**
 * File that contains the implementation of Collector. It is the child process and single-threaded
 * @author Gianmarco Petrocchi
 */

#include "../Collector.h"
#include "../utilities.h"

/**
 * Flag used to identify if the signal SIGUSR2 occurs.
 */
volatile sig_atomic_t flag_sigkill = 0;

static void handler(int signum);

int run_collector(char* socket_name){
    int fd_socket;
    ascending_queue *ascending_q = NULL;
    ascending_q = create_ascending_queue();

    struct sockaddr_un sa;
    strcpy(sa.sun_path, socket_name);
    sa.sun_family = AF_UNIX;

    struct sigaction s;
    memset(&s, 0, sizeof(s));
    s.sa_handler = SIG_IGN;

    if(sigaction(SIGUSR1, &s, NULL) == -1){
        perror("sigaction SIGUSR1");
        return -1;
    }

    if(sigaction(SIGHUP, &s, NULL) == -1){
        perror("sigaction SIGHUP");
        return -1;
    }

    if(sigaction(SIGINT, &s, NULL) == -1){
        perror("sigaction SIGINT");
        return -1;
    }

    if(sigaction(SIGTERM, &s, NULL) == -1){
        perror("sigaction SIGTERM");
        return -1;
    }

    if(sigaction(SIGQUIT, &s, NULL) == -1){
        perror("sigaction SIGQUIT");
        return -1;
    }

    if(sigaction(SIGPIPE, &s, NULL) == -1){
        perror("sigaction SIGPIPE");
        return -1;
    }

    s.sa_handler = handler;

    if(sigaction(SIGUSR2, &s, NULL) == -1){
        perror("sigaction SIGKILL");
        return -1;
    }

    if(ascending_q == NULL){
        fprintf(stderr, "Error in creating the ascending queue\n");
        return -1;
    }

    if(flag_sigkill == 1){
        shutdown_collector(ascending_q, -1, NULL);
        return -1;
    }

    if((fd_socket = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
        perror("socket");
        return -1;
    }

    if(flag_sigkill == 1){
        shutdown_collector(ascending_q, fd_socket, socket_name);
        return -1;
    }

    while(connect(fd_socket, (struct sockaddr *) &sa, sizeof(sa)) == -1){
        if(flag_sigkill == 1){
            shutdown_collector(ascending_q, fd_socket, socket_name);
            return -1;
        }else{
            if(errno == ENOENT)
                sleep(1);
            else
                return -1;
        }
    }

    while(1){
        int length_string_received;
        int length_filename;
        char* filename;
        char* string_received;
        long result;

        readn(fd_socket, &length_string_received, sizeof(int));
        string_received = malloc(sizeof(char) * length_string_received);
        readn(fd_socket, string_received, sizeof(char) * length_string_received);
        if(strncmp(string_received, "print", strlen("print")) == 0){
            fprintf(stdout, "SIGUSR1 communication:\n");
            print_ascending_queue(ascending_q);
            free(string_received);
        }else if(strncmp(string_received, "finish", strlen("finish")) == 0){
            free(string_received);
            break;
        }else{
            result = strtol(string_received, &filename, 10);
            readn(fd_socket, &length_filename, sizeof(int));

            push_ascending(&ascending_q, filename, result, length_filename);

            free(string_received);
        }
    }

    fprintf(stdout, "Final result\n");
    print_ascending_queue(ascending_q);
    free_ascending_queue(ascending_q);
    close(fd_socket);

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

/**
 * Method used to handle signal SIGUSR2
 * @param signum Integer that identifies the signal occurs
 */
static void handler(int signum){
    if(signum == SIGUSR2){
        flag_sigkill = 1;
    }
}


