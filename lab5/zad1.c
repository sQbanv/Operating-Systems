#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

void sigusr1_handler(int signum){
    printf("Received signal SIGUSR1\n");
}

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Program takes one argument\n");
        return -1;
    }
    
    char* task = argv[1];

    if(strcmp(task,"none") == 0){
        signal(SIGUSR1, SIG_DFL);
    } else if(strcmp(task,"ignore") == 0){
        signal(SIGUSR1, SIG_IGN);
    } else if(strcmp(task,"handler") == 0){
        signal(SIGUSR1, sigusr1_handler);
    } else if(strcmp(task,"mask") == 0){
        sigset_t newmask;
        sigemptyset(&newmask);
        sigaddset(&newmask, SIGUSR1);
        sigprocmask(SIG_SETMASK, &newmask, NULL);
    } else {
        printf("Unknown command\n");
    }
    
    raise(SIGUSR1);

    sigset_t pending_signals;
    sigpending(&pending_signals);

    if(sigismember(&pending_signals, SIGUSR1)){
        printf("SIGUSER1 is pending\n");
    }

    return 0;
}