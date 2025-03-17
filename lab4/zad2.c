#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>

int global = 0;

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Program takes one argument\n");
        return -1;
    }
    
    printf("%s\n", argv[0]);

    int local = 0;
    int status;

    pid_t pid = fork();

    if(pid == 0){
        printf("child process\n");
        global++;
        local++;
        printf("child pid: %d, parent pid: %d\n",getpid(),getppid());
        printf("child's local: %d, child's global: %d\n",local,global);
        status = execl("/bin/ls","ls","-l",argv[1]);
        exit(status);
    }

    wait(&status);

    printf("parent process\n");
    printf("parent pid = %d, child = %d\n",getpid(),pid);
    printf("Child exit code: %d\n",WEXITSTATUS(status));
    printf("Parent's local = %d, parent's global: %d\n",local,global);

    return 0;
}