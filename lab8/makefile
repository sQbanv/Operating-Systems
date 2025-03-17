CC=gcc
CFLAGS=-Wall -std=c17

.PHONY: all user printer clean

all: user printer

user: user_printer.h user.c
	$(CC) $(CFLAGS) -o user user.c

printer: user_printer.h printer.c
	$(CC) $(CFLAGS) -o printer printer.c

clean:
	rm user printer