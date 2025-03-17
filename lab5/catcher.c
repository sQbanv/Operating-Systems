#define _GNU_SOURCE
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/sysinfo.h>

volatile int receive_signal_count = 0;

void sigusr1_handler(int signum, siginfo_t *info, void *context){
    pid_t pid = info->si_pid;

    kill(pid,SIGUSR1);

    switch (info->si_value.sival_int)
    {
    case 1:
        receive_signal_count++;
        for(int i=1; i<=100; i++){
            printf("%d\n",i);
        }
        break;
    case 2:
        receive_signal_count++;
        printf("Receive signal %d times\n",receive_signal_count);
        break;
    case 3:
        raise(SIGKILL);
        break;
    default:
        break;
    }

}

int main(){
    printf("PID: %d\n", getpid());

    struct sigaction act;
    act.sa_sigaction = sigusr1_handler;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGUSR1);
    act.sa_flags = SA_SIGINFO;
        
    sigaction(SIGUSR1, &act, NULL);
    
    while(1){
        sleep(1);
    };

    return 0;
}