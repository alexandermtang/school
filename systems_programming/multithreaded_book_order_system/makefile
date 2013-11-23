CC=gcc
CFLAGS=-Wall -g

build:
	make bookorder.o tokenizer.o util.o bookorder

bookorder: bookorder.o tokenizer.o util.o
	$(CC) $(CFLAGS) bookorder.o tokenizer.o util.o -o bookorder

bookorder.o: bookorder.c
	$(CC) $(CFLAGS) -c bookorder.c

sorted-list.o: sorted-list.c sorted-list.h
	$(CC) $(CFLAGS) -c sorted-list.c

tokenizer.o: tokenizer.c tokenizer.h
	$(CC) $(CFLAGS) -c tokenizer.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c
	
clean:
	-rm -rf *.o bookorder *.dSYM

rebuild:
	make clean build
