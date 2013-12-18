CC=gcc
CFLAGS=-Wall -g

build:
	make search.o sorted-list.o util.o tokenizer.o search

search: search.o sorted-list.o util.o tokenizer.o
	$(CC) $(CFLAGS) search.o sorted-list.o util.o tokenizer.o -o search

search.o: search.c
	$(CC) $(CFLAGS) -c search.c

sorted-list.o: sorted-list.c sorted-list.h
	$(CC) $(CFLAGS) -c sorted-list.c

tokenizer.o: tokenizer.c tokenizer.h
	$(CC) $(CFLAGS) -c tokenizer.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

clean:
	-rm -rf *.o search *.dSYM

rebuild:
	make clean build
