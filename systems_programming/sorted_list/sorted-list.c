#ifndef SORTED_LIST_H
#define SORTED_LIST_H
/*
 * sorted-list.h
 */

#include <stdlib.h>

typedef struct Node* NodePtr;
typedef struct Node
{
    void *data;
    NodePtr next;
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

/*
 * Sorted list type.  You need to fill in the type as part of your implementation.
 */
typedef struct SortedList
{
    size_t       size;
    NodePtr      front;
    CompareFuncT cf;
} SortedList;
typedef struct SortedList* SortedListPtr;

/*
 * Iterator type for user to "walk" through the list item by item, from
 * beginning to end.  You need to fill in the type as part of your implementation.
 */
struct SortedListIterator
{
};
typedef struct SortedListIterator* SortedListIteratorPtr;




/*
 * SLCreate creates a new, empty sorted list.  The caller must provide
 * a comparator function that can be used to order objects that will be
 * kept in the list.
 *
 * If the function succeeds, it returns a (non-NULL) SortedListT object.
 * Else, it returns NULL.
 *
 * You need to fill in this function as part of your implementation.
 */

SortedListPtr SLCreate(CompareFuncT cf)
{
    SortedListPtr slp = (SortedListPtr)malloc(sizeof(SortedList));
    slp->front = NULL;
    slp->size = 0;
    slp->cf = cf;
    return slp;
}

/*
 * SLDestroy destroys a list, freeing all dynamically allocated memory.
 *
 * You need to fill in this function as part of your implementation.
 */
void SLDestroy(SortedListPtr list) {
    if (list->size == 0) {
        return;
    }
    NodePtr prevPtr = NULL;
    NodePtr ptr     = list->front;
    while (ptr != NULL) {
        prevPtr = ptr;
        ptr = ptr->next;
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
 *
 * You need to fill in this function as part of your implementation.
 */

int SLInsert(SortedListPtr list, void *newObj)
{
    if (list->size == 0)
    {
        NodePtr front = (NodePtr)malloc(sizeof(Node));
        front->data = newObj;
        front->next = NULL;
        
        list->front = front;
        list->size++;
        return 1;
    }
    
    NodePtr ptr     = list->front;
    NodePtr nextPtr = ptr->next;
    NodePtr prevPtr = NULL;
    while (ptr != NULL) {
        void* nodeData = ptr->data;
        int compare = list->cf(newObj, nodeData);
        if (compare < 0 && prevPtr == NULL) {
            NodePtr newNode = (NodePtr) malloc(sizeof(Node));
            newNode->data   = newObj;
            newNode->next   = ptr;
            list->front     = newNode;
            list->size++;
            return 1;
        } else if (compare == 0) { // Enter new node after
            NodePtr newNode = (NodePtr) malloc(sizeof(Node));
            newNode->data   = newObj;
            newNode->next   = nextPtr;
            ptr->next       = newNode;
            list->size++;
            return 1;
        } else if (compare > 0 && nextPtr == NULL) { // End of the list
            NodePtr newNode = (NodePtr) malloc(sizeof(Node));
            newNode->data   = newObj;
            newNode->next   = NULL;
            ptr->next       = newNode;
            list->size++;
            return 1;
        }
        
        void* nextData  = nextPtr->data;
        int compareNext = list->cf(newObj, nextData);
        
        if (compare > 0 && compareNext < 0) {
            NodePtr newNode = (NodePtr) malloc(sizeof(Node));
            newNode->data   = newObj;
            newNode->next   = nextPtr;
            ptr->next       = newNode;
            list->size++;
            return 1;
        }
        
        prevPtr = ptr;
        ptr     = ptr->next;
        nextPtr = ptr->next;
    }
    
    return 0;
}


/*
 * SLRemove removes a given object from a sorted list.  Sorted ordering
 * should be maintained.
 *
 * If the function succeeds, it returns 1.  Else, it returns 0.
 *
 * You need to fill in this function as part of your implementation.
 */

int SLRemove(SortedListPtr list, void *newObj)
{
    if (list->size == 0) {
        return 0;
    }
    
    NodePtr prevPtr = NULL;
    NodePtr ptr     = list->front;
    while (ptr != NULL) {
        void* nodeData = ptr->data;
        int compare = list->cf(newObj, nodeData);
        
        if (compare == 0 && prevPtr == NULL) { // Remove first element
            list->front = ptr->next;
            free(ptr);
            list->size--;
            return 1;
        } else if (compare == 0) {
            prevPtr->next = ptr->next;
            free(ptr);
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
 *
 * You need to fill in this function as part of your implementation.
 */

SortedListIteratorPtr SLCreateIterator(SortedListPtr list);


/*
 * SLDestroyIterator destroys an iterator object that was created using
 * SLCreateIterator().  Note that this function should destroy the
 * iterator but should NOT affectt the original list used to create
 * the iterator in any way.
 *
 * You need to fill in this function as part of your implementation.
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
 *
 * You need to fill in this function as part of your implementation.
 */

void *SLNextItem(SortedListIteratorPtr iter);

#endif