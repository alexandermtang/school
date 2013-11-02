#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#include "sorted-list.h"
#include "tokenizer.h"
#include "uthash.h"
#include "util.h"

struct Record {
  int id;
  char *word;
  SortedList *filenames;
  UT_hash_handle hh;
};

struct Record *records = NULL;

int djb2(unsigned char *str);
void destroyStrings(void *p);
void add_record(char *word, char *filename);
struct Record * find_record(char *word);
void delete_all_records(void);
void print_all_records(void);
void print_list(SortedList *list);
void parse_file(char *file);
SortedListPtr ll_and(SortedListPtr list1, SortedListPtr list2);

int djb2(unsigned char *str)
{
  int c, hash = 5381;

  while ((c = *str++))
    hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

  return (hash < 0) ? -hash : hash;
}

void destroyStrings(void *p) {
  char *s = (char *)p;
  free(s);
}

void add_record(char *word, char *filename)
{
  struct Record *r;

  int record_id = djb2((unsigned char *) word);

  HASH_FIND_INT(records, &record_id, r);
  // linear probing to resolve collisions
  while (r != NULL) {
    // found word
    if (strcmp(r->word, word) == 0) {
      break;
    }

    record_id++;
    HASH_FIND_INT(records, &record_id, r);
  }

  if (r == NULL) {
    // found empty spot, proceed to add
    r = (struct Record *)malloc(sizeof(struct Record));
    r->id = record_id;
    r->word = word;
    r->filenames = SLCreate(compareStrings, destroyStrings);
    SLInsert(r->filenames, filename);

    HASH_ADD_INT(records, id, r);
  } else {
    SLInsert(r->filenames, filename);
  }
}

// return NULL if not found
struct Record * find_record(char *word)
{
  struct Record *r;

  int record_id = djb2((unsigned char *) word);
  HASH_FIND_INT(records, &record_id, r);

  // possible collision, linear probing
  while (r != NULL) {
    // found word
    if (strcmp(r->word, word) == 0) {
      break;
    }

    record_id++;
    HASH_FIND_INT(records, &record_id, r);
  }

  return r;
}

void delete_all_records(void)
{
  struct Record *current_record, *tmp;

  HASH_ITER(hh, records, current_record, tmp) {
    HASH_DEL(records, current_record);
    free(current_record->word);
    SLDestroy(current_record->filenames);
    free(current_record);
  }
}

void parse_file(char *file)
{
  FILE *fp;
  fp = fopen(file, "r");

  // parse file into a hashtable of linked lists
  char *linep = NULL;
  size_t linecap = 0;
  ssize_t linelen;
  char *token, *word = NULL, *filename;

  while ((linelen = getline(&linep, &linecap, fp)) != -1) {
    TokenizerT *tokenizer = TKCreate(" \n", linep);
    token = TKGetNextToken(tokenizer);

    if (strcmp(token, "<list>") == 0) {
      free(token);
      // set current word
      token = TKGetNextToken(tokenizer);
      word = (char *)malloc(strlen(token) + 1);
      strcpy(word, token);
    } else if (strcmp(token, "</list>") == 0) {
      // do nothing
    } else {
      // iterate through filenames
      do {
        if (!isnum(token)) {
          filename = (char *)malloc(strlen(token) + 1);
          strcpy(filename, token);
          add_record(word, filename);
        }
        free(token);
      } while ((token = TKGetNextToken(tokenizer)));
    }

    free(token);
    TKDestroy(tokenizer);
  }

  if (linep) {
    free(linep);
  }

  fclose(fp);
}

void search_and(char *line)
{
  TokenizerT *tokenizer = TKCreate(" \n", line);
  char *token = TKGetNextToken(tokenizer);
  free(token);  // get rid of "sa" token
  struct Record *r;
  SortedList *list = SLCreate(compareStrings, destroyStrings);

  // initialize list to filenames of first record
  token = TKGetNextToken(tokenizer);
  r = find_record(token);
  SortedListIterator *iter = SLCreateIterator(r->filenames);
  char *file;
  while((file = (char *)SLNextItem(iter)) != NULL) {
    char *tmp = malloc(strlen(file) + 1);
    strcpy(tmp,  file);
    SLInsert(list, tmp);
  }
  SLDestroyIterator(iter);

  while ((token = TKGetNextToken(tokenizer)) != NULL) {
    r = find_record(token);

    if (r == NULL) {
      printf("String %s not found.\n", token);
      continue;
    }

    // find intersection of list and tmp_list and rewrite list
    list = ll_and(list, r->filenames);

    free(token);
  }

  print_list(list);
  print_all_records();

  SLDestroy(list);
  free(token);
  TKDestroy(tokenizer);
}

SortedListPtr ll_and(SortedListPtr list1, SortedListPtr list2)
{
    SortedListPtr andlist = SLCreate(list1->compare_func, list1->destroy_func);

    NodePtr ptr1 = list1->front;
    NodePtr ptr2 = list2->front;

    while (ptr1 != NULL && ptr2 != NULL) {
        int comp = list1->compare_func(ptr1->data, ptr2->data);

        if (comp == 0) {
            SLInsert(andlist, ptr1->data);
            ptr1 = ptr1->next;
            ptr2 = ptr2->next;
        } else if (comp < 0) {
            ptr1 = ptr1->next;
        } else {
            ptr2 = ptr2->next;
        }
    }
    // SLDestroy(list1);
    return andlist;
}

void search_or(char *line)
{
  TokenizerT *tokenizer = TKCreate(" \n", line);
  char *token = TKGetNextToken(tokenizer);
  free(token);  // get rid of "so" token
  struct Record *r;
  SortedList *list = SLCreate(compareStrings, destroyStrings);

  while ((token = TKGetNextToken(tokenizer)) != NULL) {
    r = find_record(token);

    if (r == NULL) {
      printf("String %s not found.\n", token);
      continue;
    }

    SortedListIterator *iter = SLCreateIterator(r->filenames);
    char *file;
    while((file = (char *)SLNextItem(iter)) != NULL) {
      char *tmp = malloc(strlen(file) + 1);
      strcpy(tmp,  file);
      SLInsert(list, tmp);
    }
    SLDestroyIterator(iter);

    free(token);
  }

  print_list(list);
  /*print_all_records();*/

  SLDestroy(list);
  free(token);
  TKDestroy(tokenizer);
}

void print_list(SortedList *list)
{
  if (list == NULL) {
    return;
  }

  SortedListIterator *iter = SLCreateIterator(list);
  char *item;
  while((item = (char *)SLNextItem(iter)) != NULL) {
    printf("%s\n", item);
  }
  SLDestroyIterator(iter);
}

void print_all_records(void)
{
  struct Record *s, *tmp;

  HASH_ITER(hh, records, s, tmp) {
    SortedListIterator *iter = SLCreateIterator(s->filenames);
    char *file;
    while((file = (char *)SLNextItem(iter)) != NULL) {
      printf("word %s \t id %d \t file %s\n", s->word, s->id, file);
    }
    SLDestroyIterator(iter);
  }
}

int main(int argc, char **argv)
{
  if (argc != 2) {
    printf("Error: Incorrect format, expecting:\n");
    printf("search <inverted-index file name>\n");
    return 1;
  }

  char *filename = argv[1];
  if (!file_exists(filename)) {
    printf("Error: %s does not exist.\n", filename);
    return 1;
  }

  parse_file(filename);

  print_all_records();

  unsigned int num_records;
  num_records = HASH_COUNT(records);
  printf("numrecords %u\n", num_records);

  char *linep = NULL;
  size_t linecap = 0;
  ssize_t linelen;

  printf("$ ");

  // WHAT IS THE RIGHT WAY TO READ A LINE FROM STDIN
  while ((linelen = getline(&linep, &linecap, stdin)) != -1) {
    TokenizerT *tokenizer = TKCreate(" \n", linep);
    char *cmd = TKGetNextToken(tokenizer);

    if (strcmp(cmd, "q") == 0) {
      // delete records and free everything
      delete_all_records();

      free(cmd);
      TKDestroy(tokenizer);
      return 0;
    }

    if (strcmp(cmd, "sa") == 0) {
      search_and(linep);
      printf("$ ");
      continue;
    }

    if (strcmp(cmd, "so") == 0) {
      search_or(linep);
      printf("$ ");
      continue;
    }

    printf("Error: %s: invalid command.\n", cmd);
    printf("$ ");

    free(cmd);
    TKDestroy(tokenizer);
  }

  if(linep) {
    free(linep);
  }

  return 0;
}
