CC=gcc
CFLAGS = -Wall -g -o

build:
	ar rv libsl.a sorted-list.h sorted-list.c
	$(CC) -c sorted-list.c -o sorted-list.o
	$(CC) -c main.c -o main.o
	$(CC) sorted-list.o main.o -o sl

clean:
	-rm -rf *.o sl libsl.a *.dSYM

rebuild:
	make clean build
