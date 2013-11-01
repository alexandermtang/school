CC=gcc
CFLAGS=-Wall -g

build:
	make search.o sorted-list.o util.o search

search.o: search.c
	$(CC) $(CFLAGS) -c search.c

sorted-list.o: sorted-list.c sorted-list.h
	$(CC) $(CFLAGS) -c sorted-list.c

search: search.o sorted-list.o
	$(CC) $(CFLAGS) search.o sorted-list.o util.o -o search

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

clean:
	-rm -rf *.o search *.dSYM

rebuild:
	make clean build
