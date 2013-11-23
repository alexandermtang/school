CC=gcc
CFLAGS=-Wall -g

build:
	make bookorder.o tokenizer.o bookorder

bookorder: bookorder.o tokenizer.o
	$(CC) $(CFLAGS) bookorder.o tokenizer.o -o bookorder

bookorder.o: bookorder.c
	$(CC) $(CFLAGS) -c bookorder.c

sorted-list.o: sorted-list.c sorted-list.h
	$(CC) $(CFLAGS) -c sorted-list.c

tokenizer.o: tokenizer.c tokenizer.h
	$(CC) $(CFLAGS) -c tokenizer.c

clean:
	-rm -rf *.o bookorder *.dSYM

rebuild:
	make clean build
