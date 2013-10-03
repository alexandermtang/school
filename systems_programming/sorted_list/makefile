CC=gcc
CFLAGS=-Wall -g

build:
	make main.o sorted-list.o libsl.a sl

main.o: main.c
	$(CC) $(CFLAGS) -c main.c

sorted-list.o: sorted-list.c sorted-list.h
	$(CC) $(CFLAGS) -c sorted-list.c

libsl.a: sorted-list.o main.o
	ar rv libsl.a sorted-list.o main.o

sl:
	$(CC) $(CFLAGS) sorted-list.o main.o -o sl

clean:
	-rm -rf *.o sl libsl.a *.dSYM

rebuild:
	make clean build
