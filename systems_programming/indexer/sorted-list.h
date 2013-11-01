#ifndef SORTED_LIST_H
#define SORTED_LIST_H
/*
 * sorted-list.h
 */

#include <stdlib.h>
#include <string.h>

int compareInts(void *p1, void *p2);
int compareDoubles(void *p1, void *p2);
int compareStrings(void *p1, void *p2);

typedef struct Node* NodePtr;
typedef struct Node {
    void *  data;
    NodePtr next;
    int     refCount;
} Node;

/*
 * When your sorted list is used to store objects of some type, since the
 * type is opaque to you, you will need a comparator function to order
 * the objects in your sorted list.
 *
 * You can expect a comparator function to return -1 if the 1st object is
 * smaller, 0 if the two objects are equal, and 1 if the 2nd object is
 * smaller.
 *
 * Note that you are not expected to implement any comparator functions.
 * You will be given a comparator function when a new sorted list is
 * created.
 */
typedef int (*CompareFuncT)(void *, void *);
typedef void (*DestroyFuncT)(void *);

/*
 * Sorted list type.  You need to fill in the type as part of your implementation.
 */
typedef struct SortedList {
    size_t       size;
    NodePtr      front;
    CompareFuncT compare_func;
    DestroyFuncT destroy_func;
} SortedList;
typedef struct SortedList* SortedListPtr;

/*
 * Iterator type for user to "walk" through the list item by item, from
 * beginning to end.  You need to fill in the type as part of your implementation.
 */
typedef struct SortedListIterator {
    NodePtr ptr;
    unsigned int isFront;
} SortedListIterator;
typedef struct SortedListIterator* SortedListIteratorPtr;

/*
 * SLCreate creates a new, empty sorted list.  The caller must provide
 * a comparator function that can be used to order objects that will be
 * kept in the list.
 *
 * If the function succeeds, it returns a (non-NULL) SortedListT object.
 * Else, it returns NULL.
 */
SortedListPtr SLCreate(CompareFuncT compare_func, DestroyFuncT destroy_func);

/*
 * SLDestroy destroys a list, freeing all dynamically allocated memory.
 */
void SLDestroy(SortedListPtr list);


/*
 * SLInsert inserts a given object into a sorted list, maintaining sorted
 * order of all objects in the list.  If the new object is equal to a subset
 * of existing objects in the list, then the subset can be kept in any
 * order.
 *
 * If the function succeeds, it returns 1.  Else, it returns 0.
 */
int SLInsert(SortedListPtr list, void *newObj);

NodePtr SLFind(SortedListPtr list, void *target);


/*
 * SLRemove removes a given object from a sorted list.  Sorted ordering
 * should be maintained.
 *
 * If the function succeeds, it returns 1.  Else, it returns 0.
 */
int SLRemove(SortedListPtr list, void *newObj);


/*
 * SLCreateIterator creates an iterator object that will allow the caller
 * to "walk" through the list from beginning to the end using SLNextItem.
 *
 * If the function succeeds, it returns a non-NULL SortedListIterT object.
 * Else, it returns NULL.
 */
SortedListIteratorPtr SLCreateIterator(SortedListPtr list);


/*
 * SLDestroyIterator destroys an iterator object that was created using
 * SLCreateIterator().  Note that this function should destroy the
 * iterator but should NOT affectt the original list used to create
 * the iterator in any way.
 */
void SLDestroyIterator(SortedListIteratorPtr iter);


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
void *SLNextItem(SortedListIteratorPtr iter);

#endif
