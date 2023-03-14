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
        int error = run_collector();

        if(error == 0){
            exit(EXIT_SUCCESS);
        }else{
            exit(EXIT_FAILURE);
        }
    }else {
#ifdef DEBUG
        fprintf(stderr, "Parent process %d\n\n", getpid());
#endif
        int error = run_master_thread(argc, argv, getpid());

        if(error == 0){
            exit(EXIT_SUCCESS);
        }else{
            exit(EXIT_FAILURE);
        }
    }
}
