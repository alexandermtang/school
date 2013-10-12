#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sorted-list.h"
#include "tokenizer.h"

typedef struct Term {
  char *word;
  int frequency;
} Term;

char *toLowerCase(char *str)
{
  int i = 0;
  for(i = 0; str[i]; i++) {
    str[i] = tolower(str[i]);
  }
  return str;
}

int main(int argc, char *argv[])
{
  FILE *fp;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  fp = fopen("makefile", "r");
  if (fp == NULL)
    exit(1);

  char *token;

  SortedListPtr list = SLCreate(compareStrings);

  while ((read = getline(&line, &len, fp)) != -1) {
    TokenizerT *tokenizer = TKCreate("", line);
    while ((token = TKGetNextToken(tokenizer))) {
      token = toLowerCase(token);
      SLInsert(list, (void *)token);
    }
    TKDestroy(tokenizer);
  }

  NodePtr node = SLFind(list, "o");
  if (node) {
  printf("TARGET: %s\n\n\n", (char *)node->data);
  } else {
    printf("notfound");
  }


  SortedListIteratorPtr iter = SLCreateIterator(list);
  void *item;
  while((item = SLNextItem(iter))) {
    printf("%s\n", (char *)item);
  }

  SLDestroy(list);
  SLDestroyIterator(iter);

  fclose(fp);
  exit(0);
}
