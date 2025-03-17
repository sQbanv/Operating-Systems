#include <ncurses.h>
#include <locale.h>
#include <unistd.h>
#include <stdbool.h>
#include "grid.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>

typedef struct{
	char** src;
	char** dst;
	int from;
	int to;
} thread_args;

void signal_handler(int signo){
	//Empty signal handler to wake up from pause
}

void *thread_update_grid(void *_args){
	thread_args *args = (thread_args *) _args;

	signal(SIGUSR1, signal_handler);

	while(1){
		pause();

		update_grid_thread(*args->src,*args->dst,args->from,args->to);
	}

	pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
	if(argc != 2){
		printf("Program takes 1 argument\n");
		return -1;
	}
	int n = atoi(argv[1]);
	int n_fields = GRID_WIDTH * GRID_HEIGHT;
	if(n < 1 || n > n_fields){
		printf("Number of threds must be between 1-%d\n",n_fields);
		return -1;
	}
	int fields_per_thread = n_fields / n;

	srand(time(NULL));
	setlocale(LC_CTYPE, "");
	initscr(); // Start curses mode

	char *foreground = create_grid();
	char *background = create_grid();
	char *tmp;

	init_grid(foreground);

	pthread_t threads[n];
	thread_args thread_args[n];

	int curr_field = 0;
	int residue_fields = n_fields % n;	
	for(int i=0; i<n; i++){
		thread_args[i].src = &foreground;
		thread_args[i].dst = &background;
		thread_args[i].from = curr_field;
		thread_args[i].to = curr_field + fields_per_thread;
		curr_field+=fields_per_thread;
		if(residue_fields > 0){
			thread_args[i].to += 1;
			residue_fields--;
			curr_field += 1;
		}
		if(pthread_create(&threads[i],NULL,thread_update_grid,&thread_args[i])){
			perror("pthread_create");
			exit(0);
		}
	}

	while (true)
	{
		draw_grid(foreground);
		usleep(500 * 1000);

		for(int i=0; i<n; i++){
			pthread_kill(threads[i],SIGUSR1);
		}

		tmp = foreground;
		foreground = background;
		background = tmp;
	}

	endwin(); // End curses mode
	destroy_grid(foreground);
	destroy_grid(background);

	return 0;
}
