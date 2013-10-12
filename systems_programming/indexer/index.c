#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sorted-list.h"

typedef struct Term {
    char *word;
    int frequency;
} Term;

int main(int argc, char *argv[]) {
    FILE *fp = fopen("sorted-list.c");
    if (fp == NULL) {
        fprintf(stderr, "Can't open input file in.list!\n");
        exit(1);
    }

    char line[80];

    while (!fgets(line, 80, fp) != NULL) {
        fprintf();
    }
    return 0;
}
