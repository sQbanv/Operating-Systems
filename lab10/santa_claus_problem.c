#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define NUMBER_OF_REINDEER 9
#define MAX_PRESENT 4
#define REINDEER_TIME_MIN 5
#define REINDEER_TIME_MAX 10
#define SANTA_TIME_MIN 2
#define SANTA_TIME_MAX 4

int present_time = 0;

int reindeer_counter = 0;
int is_santa_awake = 0;
int present_counter = 0;
pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t present_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t santa_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t present_cond = PTHREAD_COND_INITIALIZER;

void *reindeer(void* id){
    pthread_t tid = pthread_self();
    while(1){
        srand(time(NULL) ^ tid);
        sleep(rand() % (REINDEER_TIME_MAX + 1 - REINDEER_TIME_MIN) + REINDEER_TIME_MIN);

        pthread_mutex_lock(&reindeer_mutex);
        reindeer_counter++;
        printf("Renifer: %d czeka na Mikołaja, %lu\n",reindeer_counter,(unsigned long)tid);
        if(reindeer_counter == NUMBER_OF_REINDEER){
            printf("Renifer: Wybudzam Mikołaja, %lu\n",(unsigned long)tid);
            pthread_cond_signal(&reindeer_cond);
        }
        pthread_mutex_unlock(&reindeer_mutex);

        pthread_mutex_lock(&santa_mutex);
        while(is_santa_awake == 0){
            pthread_cond_wait(&santa_cond,&santa_mutex);
        }
        pthread_mutex_unlock(&santa_mutex);

        sleep(present_time);

        pthread_mutex_lock(&reindeer_mutex);
        reindeer_counter--;
        if(reindeer_counter == 0){
            pthread_cond_signal(&reindeer_cond);
        }
        pthread_mutex_unlock(&reindeer_mutex);

        pthread_mutex_lock(&santa_mutex);
        while(is_santa_awake == 1){
            pthread_cond_wait(&santa_cond,&santa_mutex);
        }
        pthread_mutex_unlock(&santa_mutex);
    }
}

void *santa(void* _args){
    while(1){
        srand(time(NULL));
        present_time = rand() % (SANTA_TIME_MAX + 1 - SANTA_TIME_MIN) + SANTA_TIME_MIN;

        pthread_mutex_lock(&reindeer_mutex);
        while(reindeer_counter < NUMBER_OF_REINDEER){
            pthread_cond_wait(&reindeer_cond,&reindeer_mutex);
        }
        pthread_mutex_unlock(&reindeer_mutex);
        printf("Mikołaj: Budzę się\n");

        pthread_mutex_lock(&santa_mutex);
        is_santa_awake++;
        if(is_santa_awake == 1){
            pthread_cond_broadcast(&santa_cond);
        }
        pthread_mutex_unlock(&santa_mutex);

        printf("Mikołaj: Dostarczam zabawki\n");
        sleep(present_time);
        
        pthread_mutex_lock(&present_mutex);
        present_counter++;
        if(present_counter == MAX_PRESENT){
            pthread_cond_signal(&present_cond);
        }
        pthread_mutex_unlock(&present_mutex);
        
        pthread_mutex_lock(&reindeer_mutex);
        while(reindeer_counter > 0){
            pthread_cond_wait(&reindeer_cond, &reindeer_mutex);
        }
        pthread_mutex_unlock(&reindeer_mutex);

        pthread_mutex_lock(&santa_mutex);
        is_santa_awake--;
        if(is_santa_awake == 0){
            printf("Mikołaj: Zasypiam\n");
            pthread_cond_broadcast(&santa_cond);
        }
        pthread_mutex_unlock(&santa_mutex);
    }
}

int main(){
    pthread_t santa_thread;
    pthread_t reindeer_threads[NUMBER_OF_REINDEER];

    if(pthread_create(&santa_thread,NULL,santa,NULL)){
        perror("pthread_create santa");
        exit(0);
    }

    for(int i=0; i<NUMBER_OF_REINDEER; i++){
        if(pthread_create(&reindeer_threads[i],NULL,reindeer,NULL)){
            perror("pthread_create reindeer");
            exit(0);
        }
    }

    pthread_mutex_lock(&present_mutex);
    while(present_counter < MAX_PRESENT){
        pthread_cond_wait(&present_cond, &present_mutex);
    }
    pthread_mutex_unlock(&present_mutex);

    return 0;
}