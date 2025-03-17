#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_PATH "./myfifo"
#define BUFFER_SIZE 512

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Program takes 2 arguments\n");
        return -1;
    }   

    double a = atof(argv[1]);
    double b = atof(argv[2]);

    if(a > b){
        printf("a must be lower than b\n");
        return -1;
    }

    char buffer[BUFFER_SIZE];
    mkfifo(FIFO_PATH, 0666);
    int fd;

    fd = open(FIFO_PATH, O_WRONLY);

    sprintf(buffer, "%lf %lf",a,b);
    write(fd, buffer, strlen(buffer));
    close(fd);

    fd = open(FIFO_PATH, O_RDONLY);

    read(fd,buffer,BUFFER_SIZE);

    printf("Result: %s\n",buffer);

    close(fd);

    unlink(FIFO_PATH);

    return 0;
}