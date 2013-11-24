#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <sys/queue.h>
#include <pthread.h>
#include <limits.h>

#include "uthash.h"
#include "tokenizer.h"
#include "util.h"
#include "queue.h"

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

struct CategoryQueue {
    char* category; // key
    Queue *queue;
    UT_hash_handle hh;
};

struct Customer* customers = NULL;
struct CategoryQueue* category_queues = NULL;

void add_customer(struct Customer *s) {
    HASH_ADD_INT(customers,customer_id,s);
}

struct Customer* find_customer(int customer_id) {
    struct Customer* s;
    HASH_FIND_INT(customers,&customer_id,s);
    return s;
}

Queue *find_category_queue(char *category) {
    struct CategoryQueue* q;
    HASH_FIND_STR(category_queues,category,q);
    if (q == NULL) {
        return NULL;
    }
    return q->queue;
}

void add_category_queue(char *category) {
    char *category_copy = malloc(strlen(category) + 1);
    strcpy(category_copy, category);
    struct CategoryQueue *c = malloc(sizeof(struct CategoryQueue));
    c->category = category_copy;
    c->queue = Q_create_queue();
    if (find_category_queue(category) == NULL) {
        HASH_ADD_STR(category_queues, category, c);    
    }
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

pthread_t create_consumer_thread(char *category)
{
    pthread_t consumer;
    pthread_create(&consumer, NULL, categoryFunc, category);


    return consumer;
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
        fprintf(stderr, "Error: Could not file database file.\n");
        return -1;
    }

    if (!file_exists(bookorderfile)) {
        fprintf(stderr, "Error: Could not file book order file.\n");
        return -1;
    }

    create_customers(databasefile);

    /*
    struct Customer* cust;
    for (cust = customers; cust != NULL; cust=cust->hh.next) {
        printf("customer name: %f\n",cust->balance);
    }
    */

    char* categories = argv[3];
    TokenizerT* tokenizer = TKCreate(" ", categories);
    char* tok = TKGetNextToken(tokenizer);

    if (file_exists(tok)) {
        FILE* fp = fopen(tok,"r");
        char line[LINE_MAX];

        while (fgets(line,LINE_MAX,fp) != NULL) {
            removeNewline(line);
            add_category_queue(line);

            // create category_threads
            pthread_t category_thread;
            pthread_create(&category_thread,NULL,categoryFunc,line);
            pthread_join(category_thread,NULL);
        }

        fclose(fp);
    } else {
        add_category_queue(tok);
        free(tok);

        while ((tok = TKGetNextToken(tokenizer)) != NULL) {
            add_category_queue(tok);

            // create category_thread
            pthread_t category_thread;
            pthread_create(&category_thread,NULL,categoryFunc,tok);
            pthread_join(category_thread,NULL);
            free(tok);
        }
    }

    struct CategoryQueue* q;
    for (q = category_queues; q != NULL; q = q->hh.next) {
        printf("My category is: %s\n",q->category);
    }

    TKDestroy(tokenizer);
    free(tok);

    // End of main thread of execution

    // Multi-threading begins

    pthread_t producer_thread;
    pthread_create(&producer_thread,NULL,orderFunc,bookorderfile);
    pthread_join(producer_thread,NULL);

}
