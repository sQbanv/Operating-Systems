CC=gcc
CFLAGS=-Wall -std=c17

.PHONY: all test clean

all: test

test: test.c
	$(CC) $(CFLAGS) -o test test.c

clean:
	rm test