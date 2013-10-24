#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

int file_exists(char *filename)
{
  struct stat info;
  return (stat(filename, &info) == 0);
}

void parse_file(FILE *fp)
{
  // parse file into a hashtable of linked lists
}

void search_and(void)
{

}

void search_or(void)
{

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

  FILE *fp;
  fp = fopen(filename, "r");
  parse_file(fp);

  char *linep = NULL;
  size_t linecap = 0;
  ssize_t linelen;

  printf("$ ");
  while ((linelen = getline(&linep, &linecap, stdin)) != -1) {
    // remove \n at end of linep
    linep[linelen - 1] = '\0'; linelen--;

    char *cmd = strtok(linep, " ");
    if (strcmp(cmd, "q") == 0) {
      return 0;
    }

    if (strcmp(cmd, "sa") == 0) {
      search_and();

      printf("$ ");
      continue;
    }

    if (strcmp(cmd, "so") == 0) {
      search_or();

      printf("$ ");
      continue;
    }

    printf("Error: %s: invalid command.\n", cmd);
    printf("$ ");
  }

  return 0;
}
