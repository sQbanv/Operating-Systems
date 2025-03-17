CC = gcc
CFLAGS =-Wall -std=c17

.PHONY: all shared_collatz static_collatz clean

all: static_collatz shared_collatz

shared_collatz: libcollatz.so

static_collatz: libcollatz.a

collatz.o : collatz.h collatz.c
	$(CC) $(CFLAGS) -c collatz.c

libcollatz.a: collatz.o
	ar rcs libcollatz.a collatz.o

libcollatz.o: collatz.c collatz.h
	$(CC) $(CFLAGS) -fPIC -c collatz.c -o libcollatz.o

libcollatz.so: libcollatz.o
	$(CC) $(CFLAGS) -shared -o libcollatz.so libcollatz.o

clean:
	rm *.o *.a *.so