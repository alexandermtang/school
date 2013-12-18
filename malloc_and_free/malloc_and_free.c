#include <stdlib.h>
#include <stdio.h>
#include "my_malloc.h"

#define malloc(x) my_malloc(x)
#define free(x) my_free(x)

int main(int argc, char* argv[])
{
	// Test free()ing unallocated pointer
	int x = 5;
	free(&x);

	// Test free()ing pointer that wasn't allocated with malloc()
	char *ptr = malloc(50);
	free(ptr+10);

  // Test redundant free()ing
  free(ptr);
  free(ptr);

  // Test saturation
  char *saturation = malloc(5000);

  // Test fragmentation
	char *big = malloc(4976);   // will be stored in big_block
	char *little = malloc(199); // will be stored in little_block
  free(big);
  free(little);

  	exit(0);
}
