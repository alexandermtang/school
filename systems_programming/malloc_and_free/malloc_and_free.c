#define malloc(x) mymalloc(x, __FILE__, __LINE__)
#define free(x) myfree(x, __FILE__, __LINE__)

void * mymalloc(x, __FILE__, __LINE__);
void * myfree(x, __FILE__, __LINE__);
