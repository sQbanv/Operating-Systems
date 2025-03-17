CC=gcc
CFLAGS=-Wall -std=c17

.PHONY: all countdown clean

all: countdown

countdown: countdown.c
	$(CC) $(CFLAGS) -g -o countdown countdown.c

clean:
	rm countdown