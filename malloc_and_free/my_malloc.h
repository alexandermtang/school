#ifndef MY_MALLOC_H
#define MY_MALLOC_H

#define BLOCKSIZE 5000
#define CHUNKTHRESHOLD 200

void *my_malloc( unsigned int size );

void my_free( void * p );

#endif
