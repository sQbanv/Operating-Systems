#include "user_printer.h"

int dequeue(SharedMemory *shm, int semid, char *buffer){
    struct sembuf sem_buf = {SEM_ENQUEUE, -1, 0};
    semop(semid, &sem_buf, 1);
    int flag = -1;

    if(shm->front != shm->rear){
        strcpy(buffer,shm->queue[shm->front].word);
        shm->front = (shm->front + 1) % (QUEUE_SIZE + 1);

        sem_buf.sem_num = SEM_QUEUE_SPACE;
        sem_buf.sem_op = 1;
        semop(semid, &sem_buf, 1);

        flag = 0;
    }

    sem_buf.sem_num = SEM_ENQUEUE;
    sem_buf.sem_op = 1;
    semop(semid, &sem_buf, 1);

    return flag;
}

void print_word(char *word){
    for(int i=0; i<WORD_SIZE; i++){
        printf("%c",word[i]);
        fflush(stdout);
        sleep(1);
    }
    printf("\n");
}

int main(){
    key_t shmkey;
    key_t semkey;
    int shmid;
    int semid;
    SharedMemory *shm;

    if((shmkey = ftok(SHARED_MEMORY_KEYPATH,PROJECT_ID)) == -1){
        perror("ftok");
        exit(1);
    }

    if((semkey = ftok(SEMOPHORE_KEYPATH,PROJECT_ID)) == -1){
        perror("ftok");
        exit(1);
    }

    if((shmid = shmget(shmkey,SHARED_MEMORY_SIZE,IPC_CREAT | PERMISSION)) == -1){
        perror("shmget");
        exit(1);
    }

    if((shm = (SharedMemory *)shmat(shmid,NULL,0)) == (void *)-1){
        perror("shmat");
        exit(1);
    }

    if((semid = semget(semkey,2,IPC_CREAT | PERMISSION)) == -1){
        perror("semget");
        exit(1);
    }
    
    char buffer[WORD_SIZE + 1];

    while(1){
        if(dequeue(shm,semid,buffer) != -1){
            print_word(buffer);
        }
        sleep(1);
    }

    shmdt(shm);

    return 0;
}