CC=gcc
CFLAGS=-Wall -g
DEPS=sorted-list.h sorted-list.c main.c

build: $(DEPS)
	ar rv libsl.a sorted-list.h sorted-list.c
	$(CC) -c sorted-list.c
	$(CC) -c main.c
	$(CC) $(CFLAGS) sorted-list.o main.o -o sl

clean:
	-rm -rf *.o sl libsl.a *.dSYM

rebuild: $(DEPS)
	make clean build
