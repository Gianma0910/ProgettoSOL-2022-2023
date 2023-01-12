#include "./libs/Master_Thread.h"
#include "./libs/Collector.h"

#define SOCKETNAME "./farm.sck"

int main(int argc, char* argv[]) {

    if(argc == 1){
        fprintf(stderr, "Use %s -n <nthread> -q <qlen> (-d <directory-name>) list_of_files -t <delay>\n", argv[0]);
        fprintf(stderr, "Otherwise use %s (-d <directory-name>) list_of_files\n", argv[0]);
        return -1;
    }

    int pid = fork();

    if(pid == -1){
        perror("fork");
        exit(EXIT_FAILURE);
    }else if(pid == 0){ //child process
        fprintf(stdout, "Processo figlio %d\n", getpid());
        int error = run_collector(SOCKETNAME);

        if(error == 0){
            fprintf(stdout, "Processo figlio %d terminato con successo\n", getpid());
            exit(EXIT_SUCCESS);
        }else{
            fprintf(stdout, "Processo figlio %d terminato con fallimento\n", getpid());
            exit(EXIT_FAILURE);
        }
    }else {
        fprintf(stdout, "Processo padre %d\n", getpid());
        int error = run_master_thread(argc, argv, pid, SOCKETNAME);

        if(error == 0){
            fprintf(stdout, "Processo padre %d terminato con successo\n", getpid());
            exit(EXIT_SUCCESS);
        }else{
            fprintf(stdout, "Processo padre %d terminato con fallimento\n", getpid());
            exit(EXIT_FAILURE);
        }
    }
}
