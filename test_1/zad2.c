#define _GNU_SOURCE
#include "zad2.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>

void mask()
{
    /*  Zamaskuj sygnał SIGUSR2, tak aby nie docierał on do procesu */

    sigset_t mask;
    sigemptyset(&mask); 
    sigaddset(&mask, SIGUSR2);
    sigprocmask(SIG_BLOCK, &mask, NULL);

    check_mask();
}

void process()
{
    /*  Stworz nowy process potomny i uruchom w nim program ./check_fork
        W procesie macierzystym:
            1. poczekaj 1 sekundę
            2. wyślij SIGUSR1 do procesu potomnego
            3. poczekaj na zakończenie procesu potomnego */

    pid_t pid = fork();
    if (pid == 0) { 
        execl("./check_fork", "check_fork", NULL); 
    } else {
        sleep(1);
        kill(pid, SIGUSR1); 
        wait(NULL); 
    }
}

int main()
{
    mask();
    process();

    return 0;
}
