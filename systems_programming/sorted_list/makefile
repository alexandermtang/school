CC=gcc
CFLAGS = -Wall -g -o

build: main.c
		$(CC) $(CFLAGS) sl main.c sorted-list.c
		ar rv libsl.a sl

clean:
		-rm -rf *.o sl

