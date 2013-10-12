CC=gcc
CFLAGS=-Wall -g

build:
	make index.o sorted-list.o strmap.o tokenizer.o index

index.o: index.c
	$(CC) $(CFLAGS) -c index.c

sorted-list.o: sorted-list.c sorted-list.h
	$(CC) $(CFLAGS) -c sorted-list.c

strmap.o: strmap.c strmap.h
	$(CC) $(CFLAGS) -c strmap.c

tokenizer.o: tokenizer.c tokenizer.h
	$(CC) $(CFLAGS) -c tokenizer.c

index: index.o sorted-list.o strmap.o tokenizer.o
	$(CC) $(CFLAGS) index.o sorted-list.o strmap.o tokenizer.o -o index

clean:
	-rm -rf *.o index *.dSYM

rebuild:
	make clean build
