#include <stdlib.h>
#include <stdio.h>
#include "my_malloc.h"

#define malloc(x) my_malloc(x)
#define free(x) my_free(x)

int main(int argc, char* argv[])
{

  exit(0);
}
