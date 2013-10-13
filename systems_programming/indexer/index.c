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

  char *filetoopen = "testfile";

  fp = fopen(filetoopen, "r");
  if (fp == NULL) {
    fprintf(stderr, "Error: %s does not exist\n", filetoopen);
    exit(0);
  }

  struct hash_table *table = hash_table_new(1);
  struct hash_table *table2 = hash_table_new(1);
      printf("%s %d\n", "wall", lua_hash("wall"));
      printf("%s %d\n", "cc", lua_hash("cc"));

  while ((read = getline(&line, &len, fp)) != -1) {
    TokenizerT *tokenizer = TKCreate("", line);
    char *token;
    while ((token = TKGetNextToken(tokenizer))) {
      token = toLowerCase(token);
      if(!hash_table_get(table2, token)) {
        printf("\n    inserting token2 %s\n", token);
        hash_table_store(table2, token, NULL);
      }
      printf("\ntoken %s\n", token);

      Term *t = (Term *) hash_table_get(table, token);


      if (t) {
        printf("looking for %s found %s\n", token, t->term);
        // SLInsert filename or increase count
        SortedListIteratorPtr iter = SLCreateIterator(t->list);

        Record *rec;
        while ((rec = (Record *)SLNextItem(iter))) {
          if (strcmp(rec->filename, filetoopen) == 0) {
            rec->count++;
            break;
          }
        }

        free(iter);
      } else {
        t = (Term *)malloc(sizeof(Term));
        t->term = token;
        t->list = SLCreate(compareRecords);
        printf("notfound %s\n", t->term);

        Record *r = (Record *)malloc(sizeof(Record));
        r->filename = filetoopen;
        r->count = 1;

        SLInsert(t->list, r);

        hash_table_store(table, token, t);
      }
    }
    TKDestroy(tokenizer);
  }

  // need to free(line) bc getline mallocs space for line
  free(line);


  printf("table pop %d\n", table->population);
  printf("table2 pop %d\n", table2->population);
  char **keys = hash_table_get_all_keys(table);
  char **keys2 = hash_table_get_all_keys(table2);
  int i = 0;
  qsort(keys2, table2->population, sizeof(char*), compareStrings);
  for (i = 0; i < table2->population; i++) {
    printf("table 2 key: %s\n", keys2[i]);
  }

  qsort(keys, table->population, sizeof(char*), compareStrings);
  for (i = 0; i < table->population; i++) {
    Term *t = (Term *) hash_table_get(table, keys[i]);
    printf("%s ", t->term);

    SortedListIteratorPtr iter = SLCreateIterator(t->list);

    Record *rec;
    while ((rec = (Record *)SLNextItem(iter))) {
      printf("\t\t%s %d\n", rec->filename, rec->count);
    }

    free(t->term);
    SLDestroy(t->list);
    free(t);

    SLDestroyIterator(iter);
  }

  free(keys);
  hash_table_destroy(table);

  fclose(fp);
  exit(0);
}
