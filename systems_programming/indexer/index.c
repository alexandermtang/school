/*#define _XOPEN_SOURCE 500 // comment this line out if on OSX*/
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

void print_list(FILE* fp, SortedListPtr table) {
  SortedListIteratorPtr iter = SLCreateIterator(table);
  void *item;
  while((item = SLNextItem(iter))) {
    Term *t = (Term *)item;
    fprintf(fp, "<list> %s\n", t->term);
    fflush(fp);

    SortedListIteratorPtr iter2 = SLCreateIterator(t->list);
    void *item2;
    while((item2 = SLNextItem(iter2))) {
      Record *r = (Record *)item2;
      fprintf(fp, "%s %d ", r->filename, r->count);
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

  printf("indexing %s\n", filename);

  if (input_fp == NULL) {
    fprintf(stderr, "Error: %s does not exist\n", filename);
    fclose(input_fp);
    return;
  }

  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  while ((read = getline(&line, &len, input_fp)) != -1) {
    TokenizerT *tokenizer = TKCreate("", line);
    char *token;
    while ((token = TKGetNextToken(tokenizer))) {
      token = toLowerCase(token);

      Term *t = (Term *)malloc(sizeof(Term));
      t->term = token;

      NodePtr node = SLFind(table, t);

      if (node) {
        t = (Term *) node->data;

        SortedListPtr records = t->list;
        NodePtr ptr = records->front;
        while (ptr) {
          Record *temp = (Record *)ptr->data;
          if (compareStrings(filename, temp->filename) == 0) {
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
        } else {
          Record *r = (Record *)malloc(sizeof(Record));
          r->filename = filename;
          r->count = 1;
          SLInsert(records, r);
        }
      } else {
        t->list = SLCreate(compareRecords);

        Record *r = (Record *)malloc(sizeof(Record));
        r->filename = filename;
        r->count = 1;
        SLInsert(t->list, r);

        SLInsert(table, (void *)t);
      }

    }
    TKDestroy(tokenizer);
  }

  free(line);
  fclose(input_fp);
}

void index_dir(SortedListPtr table, char *dirname) {
  DIR *dir;
  struct dirent *ent;

  dir = opendir(dirname);

  while((ent = readdir(dir)) != NULL) {
    if (ent->d_name[0] != '.') {
      struct stat info;
      lstat(ent->d_name, &info);

      if (S_ISDIR(info.st_mode)) {
        /*int length = strlen(dirname) + strlen(ent->d_name) + 1;*/
        /*char str[length];*/
        /*strcat(str, dirname);*/
        /*strcat(str, "/");*/
        /*strcat(str, ent->d_name);*/

        /*printf("%s IS DIR\n", str);*/
        /*index_dir(table, dirname);*/
      }

      if (S_ISREG(info.st_mode)) {
        index_file(table, ent->d_name);
      }
    }
  }
  closedir(dir);
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
int main(int argc, char *argv[])
{
  if (argc != 3) {
    fprintf(stderr, "Error: incorrect format, expecting: \n");
    fprintf(stderr, "./index <inverted-index file name> <directory or file name>\n");
    exit(0);
  }
  remove(TEMP_PATH_FILE);

  SortedListPtr table = SLCreate(compareTerms);

  char *input_arg = argv[2];
  char *output_file = argv[1];

  struct stat info;
  lstat(input_arg, &info);


  if (S_ISREG(info.st_mode)) {
    index_file(table, input_arg);

    FILE *output_fp;
    output_fp = fopen(output_file, "w");
    print_list(output_fp, table);
    fclose(output_fp);
  }

  if (S_ISDIR(info.st_mode)) {
    int flags = 0;
    nftw(input_arg, save_file_paths, 20, flags);

    // call index_file for each file in TEMP_PATH_FILE
    FILE *tmp;
    tmp = fopen(TEMP_PATH_FILE, "r");

    char *line = NULL;
    size_t len = 0;
    ssize_t numchars;

    while ((numchars = getline(&line, &len, tmp)) != -1) {
      // NEED TO REMOVE \n at end of line
      line[numchars - 1] = '\0'; numchars--;
      printf("open %s\n", line);
      index_file(table, line);
    }

    /*free(line);*/
    fclose(tmp);

    FILE *output_fp;
    output_fp = fopen(output_file, "w");
    print_list(output_fp, table);
    fclose(output_fp);
  }

  /*FILE *output_fp;*/
  /*output_fp = fopen(output_file, "w");*/
  // if output_file exists, need to prompt user if the want to rewrite

  /*print_list(output_fp, table);*/

  SLDestroy(table);

  /*fclose(output_fp);*/
  exit(0);
}
