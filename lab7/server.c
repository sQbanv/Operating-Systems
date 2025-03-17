#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <sys/types.h>

#define SERVER_KEY_PATHNAME "./makefile"
#define SERVER_ID 'M'
#define QUEUE_PERMISSION 0666
#define MAX_CLIENTS 10
#define MSG_SIZE 512

struct client{
    int client_id;
    int client_key;
};

struct mtext{
    int key;
    char text[MSG_SIZE];
};

struct msgbuf {
    long mtype;
    struct mtext mtext;
};

int client_id = 1;
struct client clients[MAX_CLIENTS];
int num_clients = 0;

void add_client(int client_key){
    struct msgbuf reply;
    reply.mtype = 1;
    if(num_clients < MAX_CLIENTS){
        clients[num_clients].client_id = client_id;
        clients[num_clients].client_key = client_key;
        printf("Client added: ID %d, Key %d\n", client_id, client_key);
        
        reply.mtext.key = client_id;
        strcpy(reply.mtext.text,"Successfully added to server.");
        msgsnd(client_key,&reply,sizeof(struct mtext),0);

        client_id++;
        num_clients++;
    } else {
        printf("Cannot add client, max number of clients reached.\n");
        reply.mtext.key = -1;
        strcpy(reply.mtext.text,"Cannot add client, max number of clients reached.");
        msgsnd(client_key,&reply,sizeof(struct mtext),0);
    }
}

void remove_client(int client_id){
    for(int i=0; i<num_clients; i++){
        if(clients[i].client_id == client_id){
            printf("Removing client %d from the list\n",client_id);
            for(int j=i; j < num_clients - 1; j++){
                clients[j] = clients[j+1];
            }
            num_clients--;
        }
    }
}

int main(){
    key_t server_key;
    int server_queue_id;
    struct msgbuf message;
    

    if((server_key = ftok(SERVER_KEY_PATHNAME,SERVER_ID)) == -1){
        perror("ftok");
        exit(1);
    }

    if((server_queue_id = msgget(server_key, IPC_CREAT | QUEUE_PERMISSION)) == -1){
        perror("msgget");
        exit(1);
    }

    printf("Server is running\n");

    while(1){
        if(msgrcv(server_queue_id,&message,sizeof(struct mtext),1,0) == -1){
            perror("msgrcv");
            exit(1);
        }
        printf("Server: message received.\n");

        if(strcmp(message.mtext.text,"INIT") == 0){
            add_client(message.mtext.key);
        } else if(strcmp(message.mtext.text,"EXIT") == 0) {
            remove_client(message.mtext.key);
        } else {
            for(int i=0; i<num_clients; i++){
                if(clients[i].client_id != message.mtext.key){
                    msgsnd(clients[i].client_key,&message,sizeof(struct mtext),0);
                    printf("Server: message sended to client with id: %d\n",clients[i].client_id);
                }
            }
        }
    }

    msgctl(server_queue_id, IPC_RMID, NULL);

    return 0;
}