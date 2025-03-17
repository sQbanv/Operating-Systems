CC=gcc
CFLAGS=-Wall -std=c17

.PHONY: all server client clean

all: server client

server: server.c
	$(CC) $(CFLAGS) server.c -o server

client: client.c
	$(CC) $(CFLAGS) client.c -o client

clean:
	rm -rf server