#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#define FIFO_PATH "./myfifo"
#define BUFFER_SIZE 512

double f(double x){
    return 4/(x*x + 1);
}

double calculate_integral(double a, double b, double h){
    double result = 0.0;
    double x;

    for(x=a; x<=b; x+=h){
        result += f(x) * h;
    }

    return result;
}

int main(){
    int fd;
    char buffer[BUFFER_SIZE];
    mkfifo(FIFO_PATH, 0666);

    fd = open(FIFO_PATH,O_RDONLY);

    read(fd,buffer,BUFFER_SIZE);

    close(fd);

    double a, b;

    sscanf(buffer, "%lf %lf",&a,&b);

    double result = calculate_integral(a,b,0.000001);

    sprintf(buffer,"%lf",result);

    fd = open(FIFO_PATH, O_WRONLY);

    write(fd,buffer,sizeof(buffer));

    close(fd);

    return 0;
}