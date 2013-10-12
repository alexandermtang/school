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
    FILE *fp, *ofp;
    char *line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("sorted-list.c", "r");
    ofp = fopen("asdf", "w");
    if (fp == NULL)
      exit(1);

    while ((read = getline(&line, &len, fp)) != -1) {
      printf("Line length %zu :\n", read);
      printf("%s", line);
      fprintf(ofp, "%s", line);
    }

    if (line)
        free(line);

    fclose(fp);
    fclose(ofp);
    exit(1);
}
