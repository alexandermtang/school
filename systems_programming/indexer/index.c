#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sorted-list.h"
#include "tokenizer.h"
#include "hash.h"

typedef struct Term {
  char *term;
  SortedListPtr list;
} Term;

typedef struct Record {
  char *filename;
  int count;
} Record;

int compareTerms(void *p1, void *p2)
{
	Term *i1 = (Term *)p1;
	Term *i2 = (Term *)p2;

	return compareStrings(i1->term, i2->term);
}

int compareRecords(void *p1, void *p2)
{
	Record *i1 = (Record *)p1;
	Record *i2 = (Record *)p2;

	return compareInts(&(i1->count), &(i2->count));
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

  struct hash_table *table = hash_table_new(100);

  while ((read = getline(&line, &len, fp)) != -1) {
    TokenizerT *tokenizer = TKCreate("", line);
    while ((token = TKGetNextToken(tokenizer))) {
      token = toLowerCase(token);
      Term *t = (Term *)malloc(sizeof(Term));
      t->term = token;
      if (!hash_table_get(table, token)) {
        hash_table_store(table, token, t);
      }
    }
    TKDestroy(tokenizer);
  }

  char **keys = hash_table_get_all_keys(table);
  int i = 0;
  qsort(keys, table->population, sizeof(char*), compareStrings);
  for (i = 0; i < table->population; i++) {
    printf("%s\n", keys[i]);
    free(hash_table_get(table, keys[i]));
  }

  hash_table_destroy(table);

  fclose(fp);
  exit(0);
}
