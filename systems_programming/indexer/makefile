CC=gcc
CFLAGS=-Wall -g

build:
	make index.o sorted-list.o tokenizer.o index

index.o: index.c
	$(CC) $(CFLAGS) -c index.c

sorted-list.o: sorted-list.c sorted-list.h
	$(CC) $(CFLAGS) -c sorted-list.c

tokenizer.o: tokenizer.c
	$(CC) $(CFLAGS) -c tokenizer.c

index: sorted-list.o index.o tokenizer.o
	$(CC) $(CFLAGS) sorted-list.o index.o tokenizer.o -o index

clean:
	-rm -rf *.o index libsl.a *.dSYM

rebuild:
	make clean build
