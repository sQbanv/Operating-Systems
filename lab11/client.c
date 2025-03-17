#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <signal.h>
#include <pthread.h>

#define BUFFER_SIZE 1024

int client_socket;
volatile int running = 1;

void handle_exit(int sig){
    printf("Disconnecting from server...\n");
    write(client_socket, "STOP", 4);
    close(client_socket);
    running = 0;
    exit(0);
}

void *send_handler(void *arg){
    char buffer[BUFFER_SIZE];

    while(running){
        if(fgets(buffer, BUFFER_SIZE, stdin) != NULL){
            buffer[strcspn(buffer,"\n")] = '\0';

            if(write(client_socket, buffer, strlen(buffer)) < 0){
                perror("write");
                break;
            }
        }
    }

    return NULL;
}

void *receive_handler(void *arg){
    char buffer[BUFFER_SIZE];
    ssize_t n;

    while(running){
        n = read(client_socket,buffer,BUFFER_SIZE);
        if(n > 0){
            buffer[n] = '\0';
            printf("Server response: %s\n",buffer);
        } else if(n < 0){
            perror("read error");
            break;
        } else {
            printf("Server disconnected\n");
            break;
        }
    }

    handle_exit(SIGINT);
    return NULL;
}

int main(int argc, char* argv[]){
    struct sockaddr_in server_addr;
    if(argc != 4){
        printf("Usage: %s [id] [address] [port]\n",argv[0]);
        return -1;
    }
    char *client_id = argv[1];
    char *address = argv[2];
    int port = atoi(argv[3]);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(address);

    if((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if(connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("connect");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    printf("Connecting to server %s:%d\n",address,port);

    signal(SIGINT, handle_exit);

    char buffer[BUFFER_SIZE];
    ssize_t n;

    write(client_socket, client_id, strlen(client_id));
    n = read(client_socket,buffer,BUFFER_SIZE);
    buffer[n] = '\0';
    printf("%s\n",buffer);

    pthread_t send_thread, receive_thread;
    pthread_create(&send_thread, NULL, send_handler, NULL);
    pthread_create(&receive_thread, NULL, receive_handler, NULL);

    pthread_join(send_thread, NULL);
    pthread_join(receive_thread, NULL);

    close(client_socket);
    return 0;
}