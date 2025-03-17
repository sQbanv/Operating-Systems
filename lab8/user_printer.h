#ifndef USER_PRINTER_H
#define USER_PRINTER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <time.h>

#define SHARED_MEMORY_KEYPATH "./makefile"
#define SEMOPHORE_KEYPATH "./user_printer.h"
#define PROJECT_ID 42
#define SHARED_MEMORY_SIZE 1024
#define PERMISSION 0666
#define QUEUE_SIZE 10
#define WORD_SIZE 10
#define USER_WAIT_MIN 2
#define USER_WAIT_MAX 10
#define SEM_ENQUEUE 0
#define SEM_QUEUE_SPACE 1

typedef struct{
    char word[WORD_SIZE + 1];
} PrintRequest;

typedef struct{
    PrintRequest queue[QUEUE_SIZE+1];
    int front;
    int rear;

} SharedMemory;

#endif /* USER_PRINTER_H */