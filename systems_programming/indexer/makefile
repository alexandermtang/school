CC=gcc
CFLAGS=-Wall -g

build:
	make index.o sorted-list.o index

index.o: index.c
	$(CC) $(CFLAGS) -c index.c

sorted-list.o: sorted-list.c sorted-list.h
	$(CC) $(CFLAGS) -c sorted-list.c

index: sorted-list.o index.o
	$(CC) $(CFLAGS) sorted-list.o index.o -o index

clean:
	-rm -rf *.o index libsl.a *.dSYM

rebuild:
	make clean build
