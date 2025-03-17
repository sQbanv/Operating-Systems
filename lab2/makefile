CC = gcc
CFLAGS =-Wall -std=c17 -O0
LIB = libcollatz

.PHONY: all shared_collatz static_collatz dynamic_collatz clean

all: static_collatz shared_collatz dynamic_collatz

dynamic_collatz: collatzClient.o $(LIB)/libcollatz.so
	$(CC) $(CFLAGS) -o collatzDynamicClient collatzClient.o -L./$(LIB) -Wl,-rpath,./$(LIB) -lcollatz -D DYNAMIC_COLLATZ

shared_collatz: collatzClient.o $(LIB)/libcollatz.so
	$(CC) $(CFLAGS) -o collatzSharedClient collatzClient.o -L./$(LIB) -Wl,-rpath,./$(LIB) -lcollatz 

static_collatz: collatzClient.o $(LIB)/libcollatz.a
	$(CC) $(CFLAGS) -o collatzStaticClient collatzClient.o $(LIB)/libcollatz.a

collatzClient.o: collatzClient.c $(LIB)/collatz.h
	$(CC) $(CFLAGS) -I . -c collatzClient.c

$(LIB)/collatz.o : $(LIB)/collatz.h $(LIB)/collatz.c
	$(CC) $(CFLAGS) -c $(LIB)/collatz.c -o $(LIB)/collatz.o

$(LIB)/libcollatz.a: $(LIB)/collatz.o
	ar rcs $(LIB)/libcollatz.a $(LIB)/collatz.o

$(LIB)/libcollatz.o: $(LIB)/collatz.c $(LIB)/collatz.h
	$(CC) $(CFLAGS) -fPIC -c $(LIB)/collatz.c -o $(LIB)/libcollatz.o

$(LIB)/libcollatz.so: $(LIB)/libcollatz.o
	$(CC) $(CFLAGS) -shared -o $(LIB)/libcollatz.so $(LIB)/libcollatz.o

clean:
	rm *.o $(LIB)/*.o $(LIB)/*.a $(LIB)/*.so collatzSharedClient collatzStaticClient collatzDynamicClient