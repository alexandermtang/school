/*
 * sorted-list.c
 */

#include	<string.h>
#include	<stdio.h>
#include	<stdlib.h>
#include	"sorted-list.h"

int compareInts(void *p1, void *p2)
{
	int i1 = *(int*)p1;
	int i2 = *(int*)p2;

	return i1 - i2;
}

int compareDoubles(void *p1, void *p2)
{
	double d1 = *(double*)p1;

  double d2 = *(double*)p2;

	return (d1 < d2) ? -1 : ((d1 > d2) ? 1 : 0);
}

int compareStrings(void *p1, void *p2)
{
	char *s1 = p1;
	char *s2 = p2;

	return strcmp(s1, s2);
}

int main()
{
    /*SortedListPtr slp = SLCreate(compareInts);*/
    /*int i;*/
    /*int num[10] = {0,1,2,3,4,5,6,7,8,9};*/
    /*for (i = 0; i < 10; i++) {*/
      /*printf("Insert %d\n", num[i]);*/
      /*SLInsert(slp, (void*)&num[i]);*/
    /*}*/

    /*SortedListIteratorPtr slip = SLCreateIterator(slp);*/
    /*void* item;*/
    /*i = 0;*/
    /*while( (item = SLNextItem(slip)) != NULL) {*/
      /*printf("Item %d: %d\n", i++, *(int*)item);*/
    /*}*/

    SortedListPtr slp = SLCreate(compareStrings);
    char* a = "Alexaslkdjflkjalksdflkjaslkdf";
    char* b = "Craig";
    char* c = "Tang";

    SLInsert(slp, a);
    SLInsert(slp, b);
    SLInsert(slp, c);

    SortedListIteratorPtr slip = SLCreateIterator(slp);
    void* item;
    int i = 0;
    while( (item = SLNextItem(slip)) != NULL) {
      printf("Item %d: %s\n", i++, (char*)item);
    }
    SLDestroy(slp);
    SLDestroyIterator(slip);

    return 0;
}
