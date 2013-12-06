#ifndef MY_MALLOC_H
#define TOKENIZER_H

#define BLOCKSIZE 4000

void *my_malloc( unsigned int size );

void my_free( void * p );

struct MemEntry {
	unsigned int size;
	struct MemEntry *prev, *succ;
	unsigned char isfree;
};

#endif