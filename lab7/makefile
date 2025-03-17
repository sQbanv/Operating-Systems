CC=gcc
CFLAGS=-Wall -std=c17

.PHONY: all server client clean

all: server client

server: server.c
	$(CC) $(CFLAGS) -o server server.c

client: client.c
	$(CC) $(CFLAGS) -o client client.c

clean:
	rm server client
