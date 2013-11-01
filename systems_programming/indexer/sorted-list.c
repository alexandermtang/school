#include <stdlib.h>
#include <stdio.h>
#include "sorted-list.h"

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
	char *s1 = (char *)p1;
	char *s2 = (char *)p2;

	return strcmp(s1, s2);
}

/*
 * SLCreate creates a new, empty sorted list.  The caller must provide
 * a comparator function that can be used to order objects that will be
 * kept in the list.
 *
 * If the function succeeds, it returns a (non-NULL) SortedListT object.
 * Else, it returns NULL.
 */
SortedListPtr SLCreate(CompareFuncT compare_func, DestroyFuncT destroy_func)
{
    SortedListPtr slPtr = (SortedListPtr) malloc(sizeof(SortedList));
    slPtr->front = NULL;
    slPtr->size  = 0;
    slPtr->compare_func = compare_func;
    slPtr->destroy_func = destroy_func;
    return slPtr;
}

/*
 * SLDestroy destroys a list, freeing all dynamically allocated memory.
 */
void SLDestroy(SortedListPtr list)
{
    if (list->size == 0) {
        free(list);
        return;
    }

    NodePtr prevPtr = NULL;
    NodePtr ptr     = list->front;
    while (ptr != NULL) {
        prevPtr = ptr;
        ptr = ptr->next;
        list->destroy_func(prevPtr->data);
        free(prevPtr);
    }
    free(list);
};


/*
 * SLInsert inserts a given object into a sorted list, maintaining sorted
 * order of all objects in the list.  If the new object is equal to a subset
 * of existing objects in the list, then the subset can be kept in any
 * order.
 *
 * If the function succeeds, it returns 1.  Else, it returns 0.
 */
int SLInsert(SortedListPtr list, void *newObj)
{
    if (list == NULL ||  newObj == NULL) {
      return 0;
    }

    // Initialize node to be inserted
    NodePtr newNode = (NodePtr) malloc(sizeof(Node));
    newNode->data = newObj;
    newNode->refCount = 0;

    // Insert at beginning if list is empty
    if (list->size == 0) {
        newNode->next = NULL;
        list->front = newNode;
        list->size++;
        return 1;
    }

    NodePtr ptr     = list->front;
    NodePtr nextPtr = ptr->next;
    NodePtr prevPtr = NULL;
    while (ptr) {
        void* ptrData = ptr->data;
        int compare = list->compare_func(newObj, ptrData);

        // Insert at beginning of list
        if (compare < 0 && prevPtr == NULL) {
            newNode->next = ptr;
            list->front   = newNode;
            list->size++;
            return 1;
        }

        // Insert new node after if same data
        // Do not insert duplicates
        if (compare == 0) {
            /*newNode->next = nextPtr;*/
            /*ptr->next     = newNode;*/
            /*list->size++;*/
            /*return 1;*/
          break;
        }

        // Insert at end of list
        if (compare > 0 && nextPtr == NULL) {
            newNode->next = NULL;
            ptr->next     = newNode;
            list->size++;
            return 1;
        }

        // Insert somewhere in middle of list
        void* nextData  = nextPtr->data;
        int compareNext = list->compare_func(newObj, nextData);
        // if newObj less than current and bigger than next, insert after ptr
        if (compare > 0 && compareNext < 0) {
            newNode->next = nextPtr;
            ptr->next     = newNode;
            list->size++;
            return 1;
        }

        prevPtr = ptr;
        ptr     = ptr->next;
        nextPtr = ptr->next;
    }

    // Fail to insert, free newNode
    free(newNode->data);
    free(newNode);
    return 0;
}

NodePtr SLFind(SortedListPtr list, void *target) {
    NodePtr ptr = list->front;
    while (ptr) {
        if (list->compare_func(ptr->data, target) == 0) {
            return ptr;
        }
        ptr = ptr->next;
    }
    return NULL;
}

/*
 * SLRemove removes a given object from a sorted list.  Sorted ordering
 * should be maintained.
 *
 * If the function succeeds, it returns 1.  Else, it returns 0.
 */
int SLRemove(SortedListPtr list, void *newObj)
{
    if (list->size == 0) {
        return 0;
    }

    NodePtr prevPtr = NULL;
    NodePtr ptr     = list->front;

    // CANNOT REMOVE IF ITERATOR POINTING AT IT
    while (ptr != NULL) {
        void* ptrData = ptr->data;
        int compare = list->compare_func(newObj, ptrData);

        if (compare == 0 && prevPtr == NULL && ptr->refCount == 0) { // Remove first element
            list->front = ptr->next;
            free(ptr->data);
            free(ptr);
            list->size--;
            return 1;
        } else if (compare == 0 && ptr->refCount == 0) {
            prevPtr->next = ptr->next;
            free(ptr->data);
            free(ptr);
            list->size--;
            return 1;
        }

        if (compare == 0 && prevPtr == NULL) { // Remove first element
            list->front = ptr->next;
            list->size--;
            return 1;
        } else if (compare == 0) {
            prevPtr->next = ptr->next;
            list->size--;
            return 1;
        }
        prevPtr = ptr;
        ptr     = ptr->next;
    }

    return 0;
};


/*
 * SLCreateIterator creates an iterator object that will allow the caller
 * to "walk" through the list from beginning to the end using SLNextItem.
 *
 * If the function succeeds, it returns a non-NULL SortedListIterT object.
 * Else, it returns NULL.
 */
SortedListIteratorPtr SLCreateIterator(SortedListPtr list) {
    if (list->size == 0) {
      return NULL;
    }

    SortedListIteratorPtr iter = (SortedListIteratorPtr) malloc(sizeof(SortedListIterator));
    iter->ptr = list->front;
    iter->ptr->refCount++;
    iter->isFront = 1;

    return iter;
};


/*
 * SLDestroyIterator destroys an iterator object that was created using
 * SLCreateIterator().  Note that this function should destroy the
 * iterator but should NOT affectt the original list used to create
 * the iterator in any way.
 */
void SLDestroyIterator(SortedListIteratorPtr iter) {
    if (iter->ptr != NULL) {
        iter->ptr->refCount--;
    }
    free(iter);
};


/*
 * SLNextItem returns the next object in the list encapsulated by the
 * given iterator.  It should return a NULL when the end of the list
 * has been reached.
 *
 * One complication you MUST consider/address is what happens if a
 * sorted list encapsulated within an iterator is modified while that
 * iterator is active.  For example, what if an iterator is "pointing"
 * to some object in the list as the next one to be returned but that
 * object is removed from the list using SLRemove() before SLNextItem()
 * is called.
 */
void *SLNextItem(SortedListIteratorPtr iter) {
    if (iter == NULL || iter->ptr == NULL) {
        return NULL;
    }

    if (iter->isFront) {
      iter->isFront = 0;
      return iter->ptr->data;
    }

    iter->ptr->refCount--;
    iter->ptr = iter->ptr->next;
    if (iter->ptr != NULL) {
        iter->ptr->refCount++;
    } else {
      return NULL;
    }

    void* nodeData = iter->ptr->data;
    return nodeData;
};
