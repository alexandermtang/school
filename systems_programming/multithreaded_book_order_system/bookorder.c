#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/queue.h>
#include <pthread.h>
#include <limits.h>
#include "tokenizer.h"
#include "util.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

int main(int argc, char *argv[]) {

    if (argc < 4) {
        fprintf(stderr, "Error: incorrect input format.\n");
        fprintf(stderr, "Correct format: ./bookorder <database file> <book order file> <category names>\n");
        return -1;
    }

    char* databasefile = argv[1];
    char* bookorderfile = argv[2];

    char* categories = argv[3];
    TokenizerT* tokenizer = TKCreate(" ", categories);
    char* tok = TKGetNextToken(tokenizer);

    if (file_exists(tok)) {

        FILE* fp = fopen(tok,"r");
        char line[LINE_MAX];

        while (fgets(line,LINE_MAX,fp) != NULL) {
            printf("%s",line);
        }

    } else {

        // TODO Handle the first token

        free(tok);
        while ((tok = TKGetNextToken(tokenizer)) != NULL) {


            free(tok);
        }
    }

    TKDestroy(tokenizer);
    free(tok);

}
