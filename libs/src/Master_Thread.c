/**
 * File that contains the implementation of MasterWorker. It is the parent process
 * @author Gianmarco Petrocchi
 */

#include "../Master_Thread.h"
#include "../parameters.h"

#define SOCKETNAME "./farm.sck"

/**
 * Flag used to identify the SIGUSR1 signal
 */
volatile sig_atomic_t flag_print = 0;
/**
 * Flag used to identify SIGINT, SIGQUIT, SIGHUP, SIGTERM signal
 */
volatile sig_atomic_t flag_others_signals = 0;

pthread_mutex_t mutex_socket = PTHREAD_MUTEX_INITIALIZER;

static void signal_handler(int signum);

int run_master_thread(int number_parameters, char* parameters[], int child_process){
    int option;

    struct sockaddr_un sa;
    strcpy(sa.sun_path, SOCKETNAME);
    sa.sun_family = AF_UNIX;

    /**
     * Sigaction used to assign handler function to the various signals
     */
    struct sigaction s;
    memset(&s, 0, sizeof(s));

    s.sa_handler = SIG_IGN;

    if(sigaction(SIGPIPE, &s, NULL) == -1){
        perror("sigaction SIGPIPE");
        shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, NULL);
        return -1;
    }

    s.sa_handler = signal_handler;

    if(sigaction(SIGUSR1, &s, NULL) == -1) {
        perror("sigaction SIGUSR1");
        shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, NULL);
        return -1;
    }

    if(sigaction(SIGINT, &s, NULL) == -1){
        perror("sigaction SIGINT");
        shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, NULL);
        return -1;
    }

    if(sigaction(SIGQUIT, &s, NULL) == -1){
        perror("sigaction SIGQUIT");
        shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, NULL);
        return -1;
    }

    if(sigaction(SIGTERM, &s, NULL) == -1){
        perror("sigaction SIGTERM");
        shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, NULL);
        return -1;
    }

    if(sigaction(SIGHUP, &s, NULL) == -1){
        perror("sigaction SIGHUP");
        shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, NULL);
        return -1;
    }

    while ((option = getopt(number_parameters, parameters, "n:q:d:t:")) != -1) {

        switch (option) {
            case 'n': {
                num_thread_worker = (int) isNumber(optarg);
                if (num_thread_worker == -1) {
                    fprintf(stderr, "Option -n require an integer parameter\n");
                    shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, NULL);
                    return -1;
                } else if (num_thread_worker <= 0) {
                    fprintf(stderr, "Option -n require positive integer parameter\n");
                    shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, NULL);
                    return -1;
                }
                break;
            }
            case 'q': {
                queue_length = (int) isNumber(optarg);
                if (queue_length == -1) {
                    fprintf(stderr, "Option -q require an integer parameter\n");
                    shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, NULL);
                    return -1;
                } else if (queue_length <= 0) {
                    fprintf(stderr, "Option -q require positive integer parameter\n");
                    shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, NULL);
                    return -1;
                }
                break;
            }
            case 'd': {
                directory_name = strndup(optarg, strlen(optarg));
                if (isDir(directory_name) != 0) {
                    fprintf(stderr, "Option -d require a legit directory name\n");
                    shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, NULL);
                    return -1;
                }
                break;
            }
            case 't': {
                time_delay = (int) isNumber(optarg);
                if (time_delay == -1) {
                    fprintf(stderr, "Option -t require an integer parameter\n");
                    shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, NULL);
                    return -1;
                } else if (time_delay < 0) {
                    fprintf(stderr, "Option -t require a positive integer parameter\n");
                    shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, NULL);
                    return -1;
                }
                break;
            }
            case '?': {
                fprintf(stderr, "Unknown option: %c\n", optopt);
                shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, NULL);
                return -1;
            }
            default:
                break;
        }
    }

    queue* q = NULL;
    q = create_queue(queue_length);

    if (q == NULL) {
        fprintf(stderr, "Error in creating the queue\n");
        shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, NULL);
        return -1;
    }

    int fd_socket = 0;
    fd_socket = socket(AF_UNIX, SOCK_STREAM, 0);

    if (fd_socket == -1) {
        perror("socket");
        shutdown_master_thread_prematurely(-1, -1, SOCKETNAME, q);
        return -1;
    }

    if (bind(fd_socket, (struct sockaddr *) &sa, sizeof(sa)) == -1) {
        perror("bind");
        shutdown_master_thread_prematurely(-1, fd_socket, SOCKETNAME, q);
        return -1;
    }

    if (listen(fd_socket, 1) == -1) {
        perror("listen");
        shutdown_master_thread_prematurely(-1, fd_socket, SOCKETNAME, q);
        return -1;
    }

    int fd_connected = 0;
    fd_connected = accept(fd_socket, NULL, 0);

    if(fd_connected == -1){
        perror("accept");
        shutdown_master_thread_prematurely(-1, fd_socket, SOCKETNAME, q);
        return -1;
    }

    thread_parameter* param = malloc(sizeof(thread_parameter));
    param->socket = fd_connected;
    param->queue = &q;
    param->flag_others_signals = &flag_others_signals;
    param->flag_print = &flag_print;
    param->mutex_socket = mutex_socket;

    pthread_t* thread_worker = spawn_threads(num_thread_worker, param);

    if(thread_worker == NULL){
        fprintf(stderr, "spawn_threads(): Error in creating pool of threads workers\n");
        shutdown_master_thread_prematurely(fd_connected, fd_socket, SOCKETNAME, q);
        free(param);
        return -1;
    }

    if(flag_others_signals == 1){
        shutdown_master_thread(thread_worker, child_process, fd_socket, SOCKETNAME, param);
        return -1;
    }

    for (int j = 0; j < number_parameters; j++) {
        if (j == 0) continue;
        else if (strncmp(parameters[j], "-n", 2) == 0 || strncmp(parameters[j], "-q", 2) == 0 ||
                 strncmp(parameters[j], "-t", 2) == 0 || strncmp(parameters[j], "-d", 2) == 0) {
            j++;
        } else {
            char* filename = strndup(parameters[j], strlen(parameters[j]));
            int length_filename = (int) strlen(filename);
            if (isFile(filename) != 0)
                fprintf(stderr, "%s isn't a regular file\n", filename);
            else {
                push(&q, filename, length_filename);
                msleep(time_delay);
            }
            free(filename);
        }

        if(flag_others_signals == 1){
            shutdown_master_thread(thread_worker, child_process, fd_socket, SOCKETNAME, param);
            break;
        }
    }

    if(flag_others_signals == 1){
        return 0;
    }else{
        if (directory_name != NULL) {
            scan_dir(directory_name, q, thread_worker, child_process, fd_connected, fd_socket, SOCKETNAME, param);
        }

        if(flag_others_signals == 0){
            shutdown_master_thread(thread_worker, child_process, fd_socket, SOCKETNAME, param);
            return 0;
        }
    }

    return 0;
}

pthread_t* spawn_threads(int n_threads, thread_parameter* param){
    pthread_t* thread_worker = malloc(sizeof(pthread_t) * n_threads);

    for(int i = 0; i < n_threads; i++){
        if((pthread_create(&thread_worker[i], NULL, run_thread, param)) != 0){
            perror("pthread_create");
            return NULL;
        }
    }

    return thread_worker;
}

void scan_dir(char* dirname, queue* q, pthread_t* thread_worker, int child_process, int fd_connected, int fd_socket, char* socket_name, thread_parameter* param){
    struct dirent* de;

    DIR* dir = opendir(dirname);

    if(dir == NULL){
        perror("opendir");
        return;
    }

    while((de = readdir(dir)) != NULL){

        if(strcmp(de->d_name, ".") != 0 && strcmp(de->d_name, "..") != 0){
            char* abs_path = str_concatn(dirname, "/", de->d_name, NULL);
            int length_abs_path = (int) strlen(abs_path);

            if(de->d_type != DT_DIR){
                push(&q, abs_path, length_abs_path);
                msleep(time_delay);
                free(abs_path);

                if(flag_others_signals == 1){
                    shutdown_master_thread(thread_worker, child_process, fd_socket, socket_name, param);
                    break;
                }
            }else{
                scan_dir(abs_path, q, thread_worker, child_process, fd_connected, fd_socket, socket_name, param);
                free(abs_path);
            }
        }
    }
    if(closedir(dir) != 0){
        perror("closedir");
        return;
    }
}

static void signal_handler(int signum){
    if(signum == SIGUSR1){
        flag_print++;
    }else if(signum == SIGINT || signum == SIGQUIT || signum == SIGTERM || signum == SIGHUP){
        flag_others_signals = 1;
    }
}

void shutdown_master_thread(pthread_t* thread_worker, int child_process, int fd_socket, char* socket_name, thread_parameter* param){
    for(int i = 0; i < num_thread_worker; i++) {
        push(param->queue, "finish", strlen("finish"));
    }

    for(int i = 0; i < num_thread_worker; i++){
        pthread_join(thread_worker[i], NULL);
    }

    free(thread_worker);

    char* string = "finish";
    int length = (int) strlen(string);
    ssize_t bytes_written = 0;

    pthread_mutex_lock(&mutex_socket);
    bytes_written = writen(param->socket, &length, sizeof(int));
    if(bytes_written == -1){
        perror("writen");
        shutdown_master_thread_prematurely(param->socket, fd_socket, socket_name, (*param->queue));
        pthread_mutex_unlock(&mutex_socket);
        return;
    }else if(bytes_written == 0){
        fprintf(stderr, "Socket client side closed\n");
        shutdown_master_thread_prematurely(param->socket, fd_socket, socket_name, (*param->queue));
        pthread_mutex_unlock(&mutex_socket);
        return;
    }

    bytes_written = writen(param->socket, string, sizeof(char) * length);
    if(bytes_written == -1){
        perror("writen");
        shutdown_master_thread_prematurely(param->socket, fd_socket, socket_name, (*param->queue));
        pthread_mutex_unlock(&mutex_socket);
        return;
    }else if(bytes_written == 0){
        fprintf(stderr, "Socket client side closed\n");
        shutdown_master_thread_prematurely(param->socket, fd_socket, socket_name, (*param->queue));
        pthread_mutex_unlock(&mutex_socket);
        return;
    }
    pthread_mutex_unlock(&mutex_socket);

    waitpid(child_process, NULL, 0);

    close(fd_socket);
    close(param->socket);
    remove(socket_name);
    free_queue((*param->queue));
    free(param);
    free(directory_name);

    return;
}

void shutdown_master_thread_prematurely(int fd_connected, int fd_socket, char* socket_name, queue* q){
    if(fd_connected != -1){
        close(fd_connected);
    }

    if(fd_socket != -1){
        close(fd_socket);
        remove(socket_name);
    }

    if(q != NULL){
        free_queue(q);
    }

    free(directory_name);

    return;
}