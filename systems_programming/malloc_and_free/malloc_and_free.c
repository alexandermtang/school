#include <stdlib.h>
#include <stdio.h>

#define malloc(x) my_malloc(x)
#define free(x) my_free(x)
#define BLOCKSIZE 5000

void * my_malloc(unsigned int size);
void * my_free(void *p);

void * my_malloc(unsigned int size)
{

  return NULL;
}

void * my_free(void *p)
{

  return NULL;
}

int main(void)
{

  exit(0);
}
