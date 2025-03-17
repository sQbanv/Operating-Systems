#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <time.h>

#define MAX_CLIENTS 3
#define BUFFER_SIZE 1024
#define MAX_ID_LENGTH 32

typedef struct {
    int socket;
    char id[MAX_ID_LENGTH];
} Client;

Client clients[MAX_CLIENTS];
int num_clients = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;

void handle_client(int client_socket);
void remove_client(int client_socket);
void *client_thread(void *arg);
void send_to_all(char *message, int sender_socket);
void send_to_one(char *message, char *to_client_id, int sender_socket);

int main(int argc, char* argv[]){
    int server_socket;
    struct sockaddr_in server_addr;

    if(argc != 3){
        printf("Usage: %s [server_addr] [PORT]\n",argv[0]);
        return -1;
    }
    
    char *address = argv[1];
    int port = atoi(argv[2]);

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(address);

    if((server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        perror("socket");
        exit(EXIT_FAILURE);
    }

    if((bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr))) < 0){
        perror("bind");
        exit(EXIT_FAILURE);
    }
    if(listen(server_socket, 10) < 0){
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is running on %s:%d\n",address,port);

    while(1){
        struct sockaddr_in client_addr;
        socklen_t client_addr_len = sizeof(client_addr);
        int client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
        char buffer[BUFFER_SIZE];

        if(client_socket < 0){
            perror("Accept failed");
            continue;
        }

        printf("New client connected\n");
        
        ssize_t n = read(client_socket,buffer,BUFFER_SIZE);
        buffer[n] = '\0';

        pthread_mutex_lock(&clients_mutex);

        if(num_clients < MAX_CLIENTS){
            printf("New client_id: %s\n",buffer);
            if(strlen(buffer) > MAX_ID_LENGTH){
                char *response = "Client id is too long";
                printf("%s\n",response);
                write(client_socket,response,strlen(response));
                close(client_socket);
            }
            int is_id = 0;
            for(int i=0; i<num_clients; i++){
                if(strcmp(clients[i].id,buffer) == 0){
                    is_id = 1;
                }
            }
            
            if(!is_id){
                char *response = "Successfully added to server. Enter commands below:";
                write(client_socket,response,strlen(response));
                
                

                clients[num_clients].socket = client_socket;
                strcpy(clients[num_clients].id, buffer);
                num_clients++;

                pthread_t tid;
                if(pthread_create(&tid, NULL, client_thread, &clients[num_clients - 1]) != 0){
                    perror("pthread_create");
                }
            } else {
                char *response = "Client with this id already exists";
                printf("%s\n",response);
                write(client_socket,response,strlen(response));
                close(client_socket);
            }
        } else {
            char *response = "Max clients reached. Connection refused";
            printf("%s\n",response);
            write(client_socket,response,strlen(response));
            close(client_socket);
        }

        pthread_mutex_unlock(&clients_mutex);
    }

    close(server_socket);

    return 0;
}

void *client_thread(void *arg){
    Client *client = (Client *)arg;
    char buffer[BUFFER_SIZE];
    ssize_t n;

    while ((n = read(client->socket, buffer, BUFFER_SIZE)) > 0){
        buffer[n] = '\0';
        printf("Received message from client %s: %s\n",client->id, buffer);

        char *command = strtok(buffer, " ");

        if(strcmp(command, "LIST") == 0){
            pthread_mutex_lock(&clients_mutex);
            char list[BUFFER_SIZE] = "Active clients: ";
            for(int i=0; i < num_clients; i++){
                strcat(list, clients[i].id);
                if(i < num_clients - 1){
                    strcat(list, ", ");
                }
            }
            pthread_mutex_unlock(&clients_mutex);
            write(client->socket, list, strlen(list));
        } else if(strcmp(command, "2ALL") == 0){
            char *message = strtok(NULL, "");
            
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            char datetime[50];
            snprintf(datetime,BUFFER_SIZE, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            if(message){
                char full_message[BUFFER_SIZE];
                snprintf(full_message, BUFFER_SIZE, "%s - id:%s | %s",datetime,client->id, message);
                send_to_all(full_message, client->socket);
            }
        } else if(strcmp(command, "2ONE") == 0){
            char *to_client = strtok(NULL, " ");
            char *message = strtok(NULL, "");
            
            time_t t = time(NULL);
            struct tm tm = *localtime(&t);
            char datetime[50];
            snprintf(datetime,BUFFER_SIZE, "%d-%02d-%02d %02d:%02d:%02d", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
            if(message){
                char full_message[BUFFER_SIZE];
                snprintf(full_message, BUFFER_SIZE, "%s - id:%s | %s",datetime,client->id,message);
                send_to_one(full_message, to_client, client->socket);
            }
        } else if(strcmp(command, "STOP") == 0){
            printf("Client %s disconected\n",client->id);
            break;
        } else {
            char response[BUFFER_SIZE];
            snprintf(response, BUFFER_SIZE, "Unknown command: %s", command);
            write(client->socket,response,strlen(response));
        }
    }

    if(n == 0){
        printf("Client %s disconnected\n",client->id);
    } else if(n < 0){
        perror("Read error");
    }

    pthread_mutex_lock(&clients_mutex);
    remove_client(client->socket);
    pthread_mutex_unlock(&clients_mutex);

    close(client->socket);
    return NULL;
}

void send_to_all(char *message, int sender_socket){ 
    pthread_mutex_lock(&clients_mutex);
    for(int i=0; i < num_clients; i++){
        if(clients[i].socket != sender_socket){
            write(clients[i].socket, message, strlen(message));
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void send_to_one(char *message, char *to_client_id, int sender_socket){
    pthread_mutex_lock(&clients_mutex);
    int is_client = 0;
    for(int i=0; i < num_clients; i++){
        if(strcmp(clients[i].id,to_client_id) == 0){
            is_client = 1;
            printf("Sending message to %s\n",to_client_id);
            write(clients[i].socket,message,strlen(message));
        }
    }
    if(!is_client){
        char *response = "Client not found";
        printf("Client %s not found\n",to_client_id);
        write(sender_socket,response,strlen(response));
    }
    pthread_mutex_unlock(&clients_mutex);
}

void remove_client(int client_socket){
    for(int i=0; i < num_clients; i++){
        if(clients[i].socket == client_socket){
            clients[i] = clients[num_clients - 1];
            num_clients--;
            break;
        }
    }
}