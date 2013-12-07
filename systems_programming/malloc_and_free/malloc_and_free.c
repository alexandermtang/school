#include <stdlib.h>
#include <stdio.h>
#include "my_malloc.h"

#define malloc(x) my_malloc(x)
#define free(x) my_free(x)

int main(int argc, char* argv[])
{

	// Test unallocated variable
	int x = 5;
	free(&x);

	// Test free twice
	char *name = malloc(50);
	free(name);
	free(name);

	char *overflow = malloc(4976);

  	exit(0);
}
