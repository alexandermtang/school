#define _GNU_SOURCE
#define TEMP_PATH_FILE ".file_paths.tmp"

#include <dirent.h>
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

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

	int c = compareInts(&(i2->count), &(i1->count));
  return (c != 0) ? c : compareStrings(i1->filename, i2->filename);
}

void destroy_term(void *p)
{
  Term *t = (Term *)p;
  free(t->term);
  SLDestroy(t->list);
  free(t);
}

void destroy_record(void *p)
{
  Record *r = (Record *)p;
  free(r->filename);
  free(r);
}

char *toLowerCase(char *str)
{
  int i = 0;
  for(i = 0; str[i]; i++) {
    str[i] = tolower(str[i]);
  }
  return str;
}

void print_table(FILE* fp, SortedListPtr table) {
  SortedListIteratorPtr iter = SLCreateIterator(table);
  Term *t;

  while((t = (Term *)SLNextItem(iter))) {
    fprintf(fp, "<list> %s\n", t->term);
    fflush(fp);

    SortedListIteratorPtr iter2 = SLCreateIterator(t->list);

    // manually print 1st record to avoid whitespace issues
    Record *r = (Record *)SLNextItem(iter2);
    fprintf(fp, "%s %d", r->filename, r->count);

    while((r = (Record *) SLNextItem(iter2))) {
      fprintf(fp, " %s %d", r->filename, r->count);
      fflush(fp);
    }

    fprintf(fp, "\n</list>\n\n");
    fflush(fp);

    SLDestroyIterator(iter2);
  }

  SLDestroyIterator(iter);
}

void index_file(SortedListPtr table, char *filename) {
  FILE *input_fp;
  input_fp = fopen(filename, "r");

  if (input_fp == NULL) {
    fprintf(stderr, "Error: %s does not exist\n", filename);
    fclose(input_fp);
    return;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t numchars;

  while ((numchars = getline(&line, &len, input_fp)) != -1) {
    TokenizerT *tokenizer = TKCreate("", line);
    char *token;
    while ((token = TKGetNextToken(tokenizer))) {
      token = toLowerCase(token);

      Term *t = (Term *)malloc(sizeof(Term));
      t->term = malloc(strlen(token) + 1);
      strcpy(t->term, token);

      NodePtr term_node = SLFind(table, t);

      // alloc new space for new filenames, i dont understand this
      char *file = malloc(strlen(filename) + 1);
      strcpy(file, filename);

      if (term_node) {
        free(t);
        t = (Term *) term_node->data;

        SortedListPtr records = t->list;
        NodePtr record_node = records->front;
        while (record_node) {
          Record *temp = (Record *)record_node->data;
          if (compareStrings(file, temp->filename) == 0) {
            break;
          }
          record_node = record_node->next;
        }

        if (record_node) {
          Record *temp = (Record *) record_node->data;

          Record *r = (Record *) malloc(sizeof(Record));
          r->filename = file;
          r->count = temp->count;

          SLRemove(records, temp);
          r->count++;
          SLInsert(records, r);
        } else {
          Record *r = (Record *)malloc(sizeof(Record));
          r->filename = file;
          r->count = 1;

          SLInsert(records, r);
        }
      } else {
        t->list = SLCreate(compareRecords, destroy_record);

        Record *r = (Record *) malloc(sizeof(Record));
        r->filename = file;
        r->count = 1;

        SLInsert(t->list, r);
        SLInsert(table, t);
      }
      free(token);
    }
    TKDestroy(tokenizer);
  }

  free(line);
  fclose(input_fp);
}

int save_file_paths(const char *fpath, const struct stat *sb,
             int tflag, struct FTW *ftwbuf)
{
  FILE *tmp;
  tmp = fopen(TEMP_PATH_FILE, "a");

  // fpath cannot contain ".file_paths.tmp"
  if (tflag == FTW_F && (strstr(fpath, TEMP_PATH_FILE) == NULL)) {
    fprintf(tmp, "%s\n", fpath);
  }

  fclose(tmp);

  return 0;
}

int is_hidden_file(char *filename) {
  char *token;
  TokenizerT *tokenizer = TKCreate("/", filename);

  while ((token = TKGetNextToken(tokenizer))) {
    if (token[0] == '.' && strcmp(token, ".") != 0 && strcmp(token, "..") != 0) {
      free(token);
      return 1;
    }
   free(token);
  }

  return 0;
}

int main(int argc, char *argv[])
{
  if (argc != 3) {
    fprintf(stderr, "Error: incorrect format, expecting: \n");
    fprintf(stderr, "./index <inverted-index file name> <directory or file name>\n");
    exit(0);
  }

  SortedListPtr table = SLCreate(compareTerms, destroy_term);

  char *input_arg = argv[2];
  char *output_file = argv[1];

  struct stat info;
  lstat(input_arg, &info);

  if (!(S_ISREG(info.st_mode)) && !(S_ISDIR(info.st_mode))) {
    fprintf(stderr, "Error: %s is not a file or directory", input_arg);
    exit(0);
  }

  if (S_ISREG(info.st_mode)) {
    index_file(table, input_arg);
  }

  if (S_ISDIR(info.st_mode)) {
    remove(TEMP_PATH_FILE);
    int flags = 0;
    nftw(input_arg, save_file_paths, 20, flags);

    // call index_file for each file in TEMP_PATH_FILE
    FILE *tmp;
    tmp = fopen(TEMP_PATH_FILE, "r");

    char *line = NULL;
    size_t len = 0;
    ssize_t numchars;

    while ((numchars = getline(&line, &len, tmp)) != -1) {
      // remove \n at end of line
      line[numchars - 1] = '\0'; numchars--;

      if (!is_hidden_file(line)) {
        index_file(table, line);
        /*fprintf(stdout, "Indexing %s\n", line);*/
      }
    }

    free(line);

    remove(TEMP_PATH_FILE);
    fclose(tmp);
  }

  // TODO if output_file already exists, prompt user
  // print to output
  FILE *output_fp;
  output_fp = fopen(output_file, "w");
  print_table(output_fp, table);
  fclose(output_fp);

  // NEED to fix SLDestroy to work with Record and Term structures
  SLDestroy(table);

  exit(0);
}
