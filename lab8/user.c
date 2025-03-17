#include "user_printer.h"

void random_characters(char *buffer, int n){
    char temp[n+1];
    int lower = 97;
    int upper = 122;
    for(int i=0;i<n;i++){
        temp[i] = (char) (rand() % (upper - lower + 1)) + lower;
    }
    temp[n] = '\0';
    strcpy(buffer,temp);
}

void check_queue(int semid){
    struct sembuf sem_buf = {SEM_QUEUE_SPACE,-1,0};
    semop(semid,&sem_buf,1);
}

void enqueue(SharedMemory *shm, int semid, char *word){
    struct sembuf sem_buf = {SEM_ENQUEUE,-1,0};
    semop(semid,&sem_buf,1);

    strcpy(shm->queue[shm->rear].word, word);
    shm->rear = (shm->rear + 1) % (QUEUE_SIZE + 1);

    sem_buf.sem_op = 1;
    semop(semid,&sem_buf,1);
}

void cleanup(int semid, int shmid, SharedMemory *shm){
    shmdt(shm);
    shmctl(shmid, IPC_RMID, NULL);
    semctl(semid, 0, IPC_RMID);
}

int main(int argc, char* argv[]){
    if(argc != 2){
        printf("Program takes one argument\n");
        return -1;
    }
    key_t shmkey;
    key_t semkey;
    int shmid;
    int semid;
    SharedMemory *shm;

    int n = atoi(argv[1]);

    if(n <= 0){
        printf("There must be at least one user\n");
        return -1;
    }

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

    shm->front = 0;
    shm->rear = 0;

    if((semid = semget(semkey,2,IPC_CREAT | PERMISSION)) == -1){
        perror("semget");
        exit(1);
    }

    if(semctl(semid,SEM_ENQUEUE,SETVAL,1) == -1){
        perror("semctl");
        exit(1);
    }

    if(semctl(semid,SEM_QUEUE_SPACE,SETVAL,QUEUE_SIZE) == -1){
        perror("semctl");
        exit(1);
    }

    for(int i=0; i<n; i++){
        pid_t pid = fork();
        if(pid == 0){
            srand(time(NULL) ^ getpid());
            char buffer[WORD_SIZE+1];
            while(1){
                random_characters(buffer,WORD_SIZE);
                check_queue(semid);
                enqueue(shm,semid,buffer);
                printf("User %d send word: %s\n",i+1,buffer);
                sleep((rand() % (USER_WAIT_MAX - USER_WAIT_MIN + 1)) + USER_WAIT_MIN);
            }
            exit(0);
        }
    }

    while(wait(NULL));

    cleanup(semid,shmid,shm);

    return 0;
}