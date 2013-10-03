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

    SortedListPtr slp = SLCreate(compareInts);
    char* a = "Alex";
    char* b = "Craig";
    char* c = "Tang";
    int*  d = 5;
    int*  e = 7;
    int*  f = 2;
    int*  g = 4;
    int*  h = 100;
    int*  i = 6;
    int*  j = 8;

    SLInsert(slp, (void*)&d);
    SLInsert(slp, (void*)&e);
    SLInsert(slp, (void*)&f);
    SLInsert(slp, (void*)&g);
    SLInsert(slp, (void*)&h);
    SLInsert(slp, (void*)&i);
    SLInsert(slp, (void*)&j);
    //SLRemove(slp, (void*)&h);
    //SLRemove(slp, (void*)&j);
    //SLRemove(slp, (void*)&g);
    SortedListIteratorPtr slip = SLCreateIterator(slp);
    void* item;
    int k = 0;
    while( (item = SLNextItem(slip)) != NULL) {
      if (*(int*)item == 100) {
          //SLInsert(slp, (void*)&g);
          SLRemove(slp, (void*)&g);
      }
      printf("Item %d: %d\n", k++, *(int*)item);
    }
    SLDestroy(slp);
    SLDestroyIterator(slip);

    return 0;
}
