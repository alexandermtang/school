#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sorted-list.h"
#include "tokenizer.h"

typedef struct Word {
  char *word;
  SortedListPtr list;
} Word;

typedef struct FileCount {
  char *filename;
  int frequency;
} FileCount;

int compareWords(void *p1, void *p2)
{
	Word *i1 = (Word *)p1;
	Word *i2 = (Word *)p2;

	return compareStrings(i1->word, i2->word);
}

int compareFileCounts(void *p1, void *p2)
{
	FileCount *i1 = (FileCount *)p1;
	FileCount *i2 = (FileCount *)p2;

	return compareInts(&(i1->frequency), &(i2->frequency));
}

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

  SortedListPtr list = SLCreate(compareWords);

  while ((read = getline(&line, &len, fp)) != -1) {
    TokenizerT *tokenizer = TKCreate("", line);
    while ((token = TKGetNextToken(tokenizer))) {
      token = toLowerCase(token);
      Word *w = (Word *)malloc(sizeof(Word));
      w->word = token;
      SLInsert(list, (void *)w);
    }
    TKDestroy(tokenizer);
  }

  SortedListIteratorPtr iter = SLCreateIterator(list);
  void *item;
  while((item = SLNextItem(iter))) {
    Word *ptr = (Word *)item;
    printf("%s\n", ptr->word);
  }

  SLDestroy(list);
  SLDestroyIterator(iter);

  fclose(fp);
  exit(0);
}
