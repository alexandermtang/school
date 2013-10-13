#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sorted-list.h"
#include "tokenizer.h"

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

	int c = compareInts(&(i1->count), &(i2->count));
  if (c != 0) {
    return c;
  } else {
    return compareStrings(i1->filename, i2->filename);
  }
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

  char *filetobeopened = "makefile";

  fp = fopen(filetobeopened, "r");
  if (fp == NULL)
    exit(1);

  char *token;

  SortedListPtr table = SLCreate(compareTerms);

  while ((read = getline(&line, &len, fp)) != -1) {
    TokenizerT *tokenizer = TKCreate("", line);
    while ((token = TKGetNextToken(tokenizer))) {
      token = toLowerCase(token);

      Term *t = (Term *)malloc(sizeof(Term));
      t->term = token;

      NodePtr node = SLFind(table, t);

      if (node) {
        t = (Term *) node;

        SortedListPtr records = t->list;
        NodePtr ptr = records->front;
        while (ptr) {
          Record *temp = (Record *)ptr->data;
          if (compareStrings(filetobeopened, temp->filename) == 0) {
            break;
          }
          ptr = ptr->next;
        }

        if (ptr) {
          Record *r = (Record *)malloc(sizeof(Record));
          Record *temp = (Record *)ptr->data;
          r->filename = temp->filename;
          r->count = temp->count;
          SLRemove(records, r);
          r->count++;
          SLInsert(records, r);
        }
      } else {
        t->list = SLCreate(compareRecords);

        Record *r = (Record *)malloc(sizeof(Record));
        r->filename = filetobeopened;
        r->count = 1;
        SLInsert(t->list, r);

        SLInsert(table, (void *)t);
      }

    }
    TKDestroy(tokenizer);
  }

  free(line);

  SortedListIteratorPtr iter = SLCreateIterator(table);
  void *item;
  while((item = SLNextItem(iter))) {
    Term *t = (Term *)item;
    printf("Term: %s \n", t->term);

    SortedListIteratorPtr iter2 = SLCreateIterator(t->list);
    void *item2;
    printf("Record: ");
    while((item2 = SLNextItem(iter2))) {
      Record *r = (Record *)item2;
      printf("%s %d\t", r->filename, r->count);
    }
    printf("\n");
    SLDestroyIterator(iter2);
  }

  SLDestroy(table);
  SLDestroyIterator(iter);

  fclose(fp);
  exit(0);
}
