#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/queue.h>
#include <pthread.h>
#include <limits.h>
#include "uthash.h"
#include "tokenizer.h"
#include "util.h"

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

typedef struct BookOrder {
    char* title;
    int   customer_id;
    char* category;
    float price;
} BookOrder;

struct Customer {
    char* name;
    int   customer_id; // key in hashtable
    char* address;
    char* state;
    char* zipcode;
    float balance;
    UT_hash_handle hh;
};

struct Customer* customers = NULL;

void add_customer(struct Customer *s) {
    HASH_ADD_INT(customers,customer_id,s);
}

struct Customer* find_customer(int customer_id) {
    struct Customer* s;
    HASH_FIND_INT(customers,&customer_id,s);
    return s;
}

void* orderFunc(void* arg) {

    char* orderFile = (char*)arg;

    return NULL;

}

void* categoryFunc(void* arg) {

    char* category = (char*)arg;

    pthread_mutex_lock(&lock);
    // TODO implement this



    pthread_mutex_unlock(&lock);

    return NULL;
}

void create_customers(char* databasefile)
{
    FILE* fp = fopen(databasefile,"r");
    char line[LINE_MAX];

    while (fgets(line,LINE_MAX,fp) != NULL) {

        // We tokenize by pipe | and quotation " character
        TokenizerT* tokenizer  = TKCreate("|\"",line);
        struct Customer *customer = malloc(sizeof(struct Customer));

        customer->name = TKGetNextToken(tokenizer);
        customer->customer_id = atoi(TKGetNextToken(tokenizer));
        customer->balance = atof(TKGetNextToken(tokenizer));
        customer->address = TKGetNextToken(tokenizer);
        customer->state = TKGetNextToken(tokenizer);
        customer->zipcode = TKGetNextToken(tokenizer);

        add_customer(customer);

        TKDestroy(tokenizer);
    }

    fclose(fp);
}

int main(int argc, char *argv[]) {

    if (argc < 4) {
        fprintf(stderr, "Error: incorrect input format.\n");
        fprintf(stderr, "Correct format: ./bookorder <database file> <book order file> <categories>\n");
        return -1;
    }

    char* databasefile = argv[1];
    char* bookorderfile = argv[2];

    if (!file_exists(databasefile)) {
        fprintf(stderr, "Error: Could not file database file.");
        return -1;
    }

    if (!file_exists(bookorderfile)) {
        fprintf(stderr, "Error: Could not file book order file.");
        return -1;
    }

    create_customers(databasefile);

    struct Customer* cust;
    for (cust = customers; cust != NULL; cust=cust->hh.next) {
        printf("customer name: %f\n",cust->balance);
    }

    char* categories = argv[3];
    TokenizerT* tokenizer = TKCreate(" ", categories);
    char* tok = TKGetNextToken(tokenizer);


    if (file_exists(tok)) {

        FILE* fp = fopen(tok,"r");
        char line[LINE_MAX];

        while (fgets(line,LINE_MAX,fp) != NULL) {

            pthread_t categoryThread;

            char* category = line;
            removeNewline(category);

            // printf("%s",category);
            pthread_create(&categoryThread,NULL,categoryFunc,line);

        }

        fclose(fp);

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
