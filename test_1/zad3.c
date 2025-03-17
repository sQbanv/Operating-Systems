#include "zad3.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

void readwrite(int pd, size_t block_size);

void createpipe(size_t block_size)
{
    /* Utwórz potok nienazwany */

    /* Odkomentuj poniższe funkcje zamieniając ... na deskryptory potoku */
    // check_pipe(...);
    // check_write(..., block_size, readwrite);

    int fd[2]; 
    pipe(fd);

    check_pipe(fd); 

    check_write(fd[1], block_size, readwrite); 
}

void readwrite(int write_pd, size_t block_size)
{
    /* Otworz plik `unix.txt`, czytaj go po `block_size` bajtów
    i w takich `block_size` bajtowych kawałkach pisz do potoku `write_pd`.*/

    /* Zamknij plik */

    FILE *file = fopen("unix.txt", "r");

    char buffer[block_size];
    while(fgets(buffer,block_size,file) != NULL){
        write(write_pd, buffer, block_size);
    }

    
    fclose(file);
}

int main()
{
    srand(42);
    size_t block_size = rand() % 128;
    createpipe(block_size);

    return 0;
}
