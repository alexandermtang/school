CC=gcc
CFLAGS=-Wall -g

build:
	make bookorder.o tokenizer.o util.o queue.o bookorder

bookorder: bookorder.o tokenizer.o util.o queue.o
	$(CC) $(CFLAGS) bookorder.o tokenizer.o util.o queue.o -o bookorder -l pthread

bookorder.o: bookorder.c
	$(CC) $(CFLAGS) -c bookorder.c

sorted-list.o: sorted-list.c sorted-list.h
	$(CC) $(CFLAGS) -c sorted-list.c

tokenizer.o: tokenizer.c tokenizer.h
	$(CC) $(CFLAGS) -c tokenizer.c

util.o: util.c util.h
	$(CC) $(CFLAGS) -c util.c

queue.o: queue.c queue.h
	$(CC) $(CFLAGS) -c queue.c
	
clean:
	-rm -rf *.o bookorder *.dSYM

rebuild:
	make clean build
