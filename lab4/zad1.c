#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int main(int argc, char *argv[]){
    if(argc != 2){
        printf("Program takes one argument (int)\n");
        return -1;
    }
    
    int n = atoi(argv[1]);

    if(n < 0){
        printf("Number of processes must be higher or equal 0\n");
    }

    for(int i=0; i<n; i++){
        pid_t child_pid;
        child_pid = fork();
        if(child_pid == 0){
            printf("PPID: %d; PID: %d\n", (int)getppid(), (int)getpid());
            exit(0);
        }
    }

    while(wait(0) > 0);

    printf("%d\n",n);
    return 0;
}