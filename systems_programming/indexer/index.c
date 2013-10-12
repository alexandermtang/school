#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sorted-list.h"

typedef struct Term {
    char *word;
    int frequency;
} Term;

int main(int argc, char *argv[])
{
    FILE *fp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("sorted-list.c", "r");
    if (fp == NULL)
      exit(1);

    while ((read = getline(&line, &len, fp)) != -1) {
      printf("Line length %zu :\n", read);
      printf("%s", line);
    }

    if (line)
        free(line);

    fclose(fp);
    exit(1);
}
