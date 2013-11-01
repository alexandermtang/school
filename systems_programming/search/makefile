CC=gcc
CFLAGS=-Wall -g

build:
	make search.o sorted-list.o tokenizer.o search

search.o: search.c
	$(CC) $(CFLAGS) -c search.c

sorted-list.o: sorted-list.c sorted-list.h
	$(CC) $(CFLAGS) -c sorted-list.c

search: search.o sorted-list.o
	$(CC) $(CFLAGS) search.o sorted-list.o -o search

clean:
	-rm -rf *.o search *.dSYM

rebuild:
	make clean build
