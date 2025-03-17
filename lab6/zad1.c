#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define BUFFER_SIZE 512

double f(double x){
    return 4/(x*x + 1);
}

double calculate_integral(double start, double end, double width){
    double sum = 0.0;
    double x;

    for(x = start; x < end; x += width){
        sum += f(x) * width;
    }

    return sum;
}

int main(int argc, char* argv[]){
    if(argc != 3){
        printf("Program takes 2 arguments\n");
        return -1;
    }

    double result = 0.0;

    double h = strtod(argv[1],NULL);
    int n = atoi(argv[2]);
    double interval = 1.0 / n;

    if(h * n > 1){
        printf("Bad arguments h * n must be less than 1\n");
        return -1;
    }

    int pipes_fd[n][2];

    for(int i=0; i<n; i++){
        pipe(pipes_fd[i]);
        pid_t pid = fork();
        if(pid == 0){
            close(pipes_fd[i][0]);
            double start = i * interval;
            double end = (i + 1) * interval;
            double partial_sum = calculate_integral(start, end, h);
            write(pipes_fd[i][1],&partial_sum,sizeof(double));
            exit(0);
        } else {
            double partial_sum;
            close(pipes_fd[i][1]);
            read(pipes_fd[i][0],&partial_sum,sizeof(double));
            result += partial_sum;
        }
    }

    printf("Result: %f\n",result);

    return 0;
}