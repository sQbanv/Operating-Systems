#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Program takes 2 arguments\n");
        return -1;
    }
    
    pid_t pid = atoi(argv[1]);

    int signal_value = atoi(argv[2]);

    union sigval value;
    value.sival_int = signal_value;

    sigqueue(pid, SIGUSR1, value);

    pause();

    return 0;
}