#include "./libs/Master_Thread.h"
#include "./libs/Collector.h"

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
        fprintf(stdout, "Child process %d\n\n", getpid());
        int error = run_collector(getppid());

        if(error == 0){
            fprintf(stdout, "Child process %d has terminated successfully\n", getpid());
            exit(EXIT_SUCCESS);
        }else{
            fprintf(stdout, "Child process %d has terminated unsuccessfully\n", getpid());
            exit(EXIT_FAILURE);
        }
    }else {
        fprintf(stdout, "Parent process %d\n\n", getpid());
        int error = run_master_thread(argc, argv, pid);

        if(error == 0){
            fprintf(stdout, "Parent process %d has terminated successfully\n", getpid());
            exit(EXIT_SUCCESS);
        }else{
            fprintf(stdout, "Parent process %d has terminated unsuccessfully\n", getpid());
            exit(EXIT_FAILURE);
        }
    }
}
