CC=gcc
CFLAGS = -Wall -g -o

build: main.c sorted-list.h
		$(CC) $(CFLAGS) sl main.c
		ar rv libsl.a sorted-list.h

clean:
		-rm -rf *.o sl libsl.a *.dSYM

rebuild: main.c sorted-list.h
		make clean build
