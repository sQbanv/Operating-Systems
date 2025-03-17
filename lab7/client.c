#define _POSIX_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>

#define SERVER_KEY_PATHNAME "./makefile"
#define SERVER_ID 'M'
#define QUEUE_PERMISSION 0666
#define MSG_SIZE 512

struct mtext{
    int key;
    char text[MSG_SIZE];
};

struct msgbuf {
    long mtype;
    struct mtext mtext;
};


int main(){
    key_t server_key;
    key_t client_key = IPC_PRIVATE;
    int server_queue_id;
    int client_queue_id;
    int client_id_on_server;
    struct msgbuf message;
    char buffer[MSG_SIZE];

    if((server_key = ftok(SERVER_KEY_PATHNAME,SERVER_ID)) == -1){
        perror("ftok");
        exit(1);
    }

    if((server_queue_id = msgget(server_key,IPC_CREAT | QUEUE_PERMISSION)) == -1){
        perror("msgget");
        exit(1);
    }

    if((client_queue_id = msgget(client_key,IPC_CREAT | QUEUE_PERMISSION)) == -1){
        perror("msgget");
        exit(1);
    }

    message.mtype = 1;
    message.mtext.key = client_queue_id;
    strcpy(message.mtext.text,"INIT");
     
    msgsnd(server_queue_id,&message,sizeof(struct mtext),0);

    if(msgrcv(client_queue_id,&message,sizeof(struct mtext),1,0) == -1){
        perror("msgrsv");
        exit(1);
    }

    printf("%s\n",message.mtext.text);
    if(message.mtext.key == -1){
        return -1;
    }
    client_id_on_server = message.mtext.key;

    pid_t pid = fork();
    if(pid == 0){
        while(1){
            if(msgrcv(client_queue_id,&message,sizeof(struct mtext),1,0) == -1){
                perror("msgrsv");
                exit(1);
            }
            printf("%s\n",message.mtext.text);
        }
    } else {
        while(1){
            if(fgets(buffer,MSG_SIZE,stdin) != NULL){
                buffer[strcspn(buffer, "\n")] = '\0';
                message.mtext.key = client_id_on_server;
                strcpy(message.mtext.text,buffer);
                msgsnd(server_queue_id,&message,sizeof(struct mtext),0);
                if(strcmp(buffer,"EXIT") == 0){
                    kill(pid, SIGTERM);
                    break;
                }
        }
    }
    }

    msgctl(client_queue_id,IPC_RMID, NULL);

    return 0;
}