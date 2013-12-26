CC=gcc
CFLAGS=-Wall -g

build:
	make my_malloc.o malloc_and_free.o malloc_and_free

malloc_and_free: my_malloc.o malloc_and_free.o
	$(CC) $(CFLAGS) my_malloc.o malloc_and_free.o -o malloc_and_free

my_malloc.o: my_malloc.c my_malloc.h
	$(CC) $(CFLAGS) -c my_malloc.c

malloc_and_free.o: malloc_and_free.c
	$(CC) $(CFLAGS) -c malloc_and_free.c

clean:
	-rm -rf *.o malloc_and_free test *.dSYM

build_test:
	make my_malloc.o test.o test

test.o:
	$(CC) $(CFLAGS) -c test.c

test: my_malloc.o
	$(CC) $(CFLAGS) my_malloc.o test.o -o test

rebuild:
	make clean build
