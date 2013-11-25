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

#define FALSE 0
#define TRUE 1

// Will be used for writing to output stream and database
pthread_mutex_t file_lock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t database_lock = PTHREAD_MUTEX_INITIALIZER;

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

struct CategoryThread {
    pthread_t thread;
    struct CategoryThread *next;
};

struct Customer* customers = NULL;
struct CategoryQueue* category_queues = NULL;

void add_customer(struct Customer *s) 
{
    HASH_ADD_INT(customers,customer_id,s);
}

struct Customer* find_customer(int customer_id) 
{
    struct Customer* s;
    HASH_FIND_INT(customers,&customer_id,s);
    return s;
}

Queue *find_category_queue(char *category) 
{
    struct CategoryQueue* q;
    HASH_FIND_STR(category_queues,category,q);
    if (q == NULL) {
        return NULL;
    }
    return q->queue;
}

void add_category_queue(char *category) 
{
    char *category_copy = malloc(strlen(category) + 1);
    strcpy(category_copy, category);
    struct CategoryQueue *c = malloc(sizeof(struct CategoryQueue));
    c->category = category_copy;
    c->queue = Q_create_queue();
    if (find_category_queue(category) == NULL) {
        HASH_ADD_KEYPTR( hh, category_queues, c->category, strlen(c->category), c );   
    }
}

void* orderFunc(void* arg) 
{   
    char* order_file = (char*)arg;

    FILE* fp = fopen(order_file, "r");

    char line[LINE_MAX];
    while (fgets(line,LINE_MAX,fp) != NULL) {
        TokenizerT *tokenizer = TKCreate("|\n\r",line);

        struct BookOrder *order = malloc(sizeof(struct BookOrder));
        order->title = TKGetNextToken(tokenizer);
        order->price = atof(TKGetNextToken(tokenizer));
        order->customer_id = atoi(TKGetNextToken(tokenizer));
        order->category = TKGetNextToken(tokenizer);
        // printf("order category %s\n", order->category+1); // Note: +1 because text isn't formatted properly

        Queue *q = find_category_queue(order->category+1);

        if (!q) {
            printf("Category %s does not exist.",order->category+1);
            return NULL;
        }

        pthread_mutex_lock(&q->mutex);
        Q_enqueue(q, (void *)order);
        /*
        while (q->length > 0) {
            pthread_cond_signal(&q->dataAvailable); // Shout at consumer
            pthread_cond_wait(&q->spaceAvailable, &q->mutex);
        }
        */
        //printf("%s thread got order %s\n", order->category, order->title);
        pthread_mutex_unlock(&q->mutex);

        TKDestroy(tokenizer);
    }

    // Notify threads about end of stream
    struct CategoryQueue *q;
    for (q = category_queues; q != NULL; q = q->hh.next) {
        pthread_mutex_lock(&q->queue->mutex);
        q->queue->isopen = FALSE;
        pthread_mutex_unlock(&q->queue->mutex);
    }

    fclose(fp);

    // listen for condition variable
    // fprintf(stdout,"Thread %s has exited.\n","PRODUCER");

    return NULL;
}

void* categoryFunc(void* arg) 
{
    char* category = (char*)arg;
    // printf("Spawn thread for %s\n",category);
    Queue* q = find_category_queue(category);
    /*
    if (!q) {
        //printf("Shouldn't ever happen.");
        return NULL;
    }
    */

    while (q->isopen || q->length > 0) {
        pthread_mutex_lock(&q->mutex);

        // printf("My isopen is: %d\n",q->isopen);
        // printf("My queue length is: %d\n",q->length);

        if (q->length == 0) {
            pthread_mutex_unlock(&q->mutex);
            continue;
            //pthread_cond_signal(&q->spaceAvailable);
            //pthread_cond_wait(&q->dataAvailable, &q->mutex);
        }

        pthread_mutex_lock(&database_lock);
        struct BookOrder *order = (struct BookOrder*)Q_dequeue(q);
        struct Customer* cust = find_customer(order->customer_id);
        cust->balance = cust->balance - order->price;

        pthread_mutex_lock(&file_lock);
        fprintf(stdout,"-----------------------------------\n");
        fprintf(stdout,"### Order Summary ###\n");
        fprintf(stdout,"Customer name: %s\n",cust->name);
        fprintf(stdout,"Customer id: %d\n",cust->customer_id);
        if (cust->balance > 0) {
            fprintf(stdout,"Balance after purchase: %.2f\n\n",cust->balance);
        } else {
            fprintf(stdout,"Order rejeceted, remaining balance: %.2f\n\n",cust->balance+order->price);
        }

        if (cust->balance < 0) {
            fprintf(stdout,"### Rejected Orders ###\n");
            fprintf(stdout, "Title of book: %s\n",order->title);
            fprintf(stdout,"Cost of book: %.2f\n\n",order->price);
            cust->balance = cust->balance+order->price;
        } else {
            fprintf(stdout,"### Accepted Orders ###\n");
            fprintf(stdout,"Title of book: %s\n",order->title);
            fprintf(stdout,"Cost of book: %.2f\n\n",order->price);
        }

        free(order->title);
        free(order->category);
        free(order);
        pthread_mutex_unlock(&file_lock);
        pthread_mutex_unlock(&database_lock);

        pthread_mutex_unlock(&q->mutex);
    }

    pthread_mutex_destroy(&q->mutex);

    // fprintf(stdout,"Thread %s has exited.\n",category);

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

    struct CategoryThread *category_threads = NULL;

    if (file_exists(tok)) {
        FILE* fp = fopen(tok,"r");
        char line[LINE_MAX];

        while (fgets(line,LINE_MAX,fp) != NULL) {
            removeNewline(line);
            add_category_queue(line);
            // Gonna cause memory leak, need to fix.
            char* category = malloc(strlen(line)+1);
            strcpy(category,line);

            // create category_threads
            pthread_t category_thread;
            pthread_create(&category_thread,NULL,categoryFunc,category);

            struct CategoryThread *cat_thread = malloc(sizeof(struct CategoryThread));
            cat_thread->thread = category_thread;
            if (category_threads == NULL) {
                category_threads = cat_thread;
                cat_thread->next = NULL;
            } else {
                cat_thread->next = category_threads;
                category_threads = cat_thread;
            }
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

            struct CategoryThread *cat_thread = malloc(sizeof(struct CategoryThread));
            cat_thread->thread = category_thread;
            if (category_threads == NULL) {
                category_threads = cat_thread;
                cat_thread->next = NULL;
            } else {
                cat_thread->next = category_threads;
                category_threads = cat_thread;
            }

            free(tok);
        }
    }

    /*
    struct CategoryQueue* q;
    for (q = category_queues; q != NULL; q = q->hh.next) {
        // printf("My category is: %s %d\n",q->category, q->queue->length);
    }
    */

    TKDestroy(tokenizer);
    free(tok);

    // End of main thread of execution

    // Multi-threading begins

    pthread_t producer_thread;
    pthread_create(&producer_thread,NULL,orderFunc,bookorderfile);

    struct CategoryThread *ptr = NULL; 
    for (ptr = category_threads; ptr != NULL; ptr=ptr->next) {
        pthread_join(ptr->thread,NULL);
    }
    pthread_join(producer_thread,NULL);

    // Might lead to problems.
    ptr = category_threads;
    while (ptr != NULL) {
        struct CategoryThread *temp = ptr;
        ptr = ptr->next;
        free(temp);
    }
    category_threads = NULL;

    struct Customer *c, *tmp;
    HASH_ITER(hh,customers,c,tmp) {
        HASH_DEL(customers,c);
        free(c->name);
        free(c->address);
        free(c->zipcode);
        free(c->state);
        free(c);
    }

    struct CategoryQueue *q, *tmp2;
    HASH_ITER(hh,category_queues,q,tmp2) {
        HASH_DEL(category_queues,q);
        Q_destroy(q->queue);
        free(q->category);
        free(q->queue);
        free(q);
    }

    pthread_mutex_destroy(&file_lock);
    pthread_mutex_destroy(&database_lock);

    return 0;
}
