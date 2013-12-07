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

	char *name = malloc(50);
	free(name+10);

  	exit(0);
}
