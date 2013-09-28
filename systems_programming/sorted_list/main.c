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
  SortedListPtr slp = SLCreate(compareInts);
  int i;
  for (i = 0; i < 100; i+=10) {
    int num = rand();
    printf("Insert %d\n", i);
    SLInsert(slp, (void*)&i);
  }

  /*printf("Item 1 %d\n", *(int*)slp->front->data);*/
  /*printf("Item 2 %d\n", *(int*)slp->front->next->data);*/
  /*printf("Item 3 %d\n", *(int*)slp->front->next->next->data);*/
  /*printf("Item 4 %d\n", *(int*)slp->front->next->next->next->data);*/

  SortedListIteratorPtr slip = SLCreateIterator(slp);
  void* item;
  i = 0;
  while( (item = SLNextItem(slip)) != NULL) {
    printf("Item %d: %d\n", i++, *(int*)item);
  }

  return 0;
}
