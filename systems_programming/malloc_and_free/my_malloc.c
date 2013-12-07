#include "my_malloc.h"
#include <stdio.h>

static char big_block[BLOCKSIZE];

struct MemEntry {
	unsigned int size;
	unsigned char isfree;
	struct MemEntry *prev, *succ;
};

// return a pointer to the memory buffer requested
void* my_malloc(unsigned int size)
{
	static int 		initialized = 0;
	static struct MemEntry *root;
	struct MemEntry *p;
	struct MemEntry *succ;
	
	if(!initialized)	// 1st time called
	{
		// printf("sizeof MemEntry: %zu\n",sizeof(struct MemEntry));
		// create a root chunk at the beginning of the memory block
		root = (struct MemEntry*)big_block;
		root->prev = root->succ = 0;
		root->size = BLOCKSIZE - sizeof(struct MemEntry);
		root->isfree = 1;
		initialized = 1;
	}
	p = root;
	do
	{
		if(p->size < size)
		{
			// the current chunk is smaller, go to the next chunk
			p = p->succ;
			// Check for saturation
			if (p == 0) {
				printf("Error: There is no memory chunk that can fit your data.\n");
				return NULL;
			}
		}
		else if(!p->isfree)
		{
			// this chunk was taken, go to the next
			p = p->succ;
		}
		else if(p->size < (size + sizeof(struct MemEntry)))
		{
			// this chunk is free and large enough to hold data, 
			// but there's not enough memory to hold the HEADER of the next chunk
			// don't create any more chunks after this one
			p->isfree = 0;
			return (char*)p + sizeof(struct MemEntry);
		}
		else
		{
			// take the needed memory and create the header of the next chunk
			succ = (struct MemEntry*)((char*)p + sizeof(struct MemEntry) + size);
			succ->prev = p;
			succ->succ = p->succ;
			//begin add
			if(p->succ != 0)
				p->succ->prev = succ;
			p->succ = succ;
			succ->isfree = 1;
			//end add
			
			succ->size = p->size - sizeof(struct MemEntry) - size;
			p->size = size;
			p->isfree = 0;
			return (char*)p + sizeof(struct MemEntry);
		}
	} while(p != 0);
	
	return 0;
}


// free a memory buffer pointed to by p
void my_free(void *p)
{

	// Note: Maybe there's a better solution
	if (p < (void*)big_block || p > ((void*)big_block + BLOCKSIZE)) {
		fprintf(stderr,"Error: Cannot free pointer that was not allocated "
					   "in file %s at line %d.\n",__FILE__,__LINE__);
		return;
	}

	struct MemEntry *ptr;
	struct MemEntry *prev;
	struct MemEntry *succ;
	
	int isMemEntry = 0;
	ptr = (struct MemEntry*)big_block;
	while (ptr != 0) {
		// printf("ptr at memory address: %p\nTrying to free: %p\n\n",ptr,p);
		if (p == (void*)ptr+sizeof(struct MemEntry)) {
			isMemEntry = 1;
			break;
		}

		ptr = ptr->succ;
	}

	if (!isMemEntry) {
		fprintf(stderr,"Error: Cannot free pointer that was not returned by malloc "
					   "in file %s at line %d.\n",__FILE__,__LINE__);
		return;
	}

	ptr = (struct MemEntry*)((char*)p - sizeof(struct MemEntry));

	if (ptr->isfree) {
		fprintf(stderr,"Error: Cannot free pointer that was already freed "
					   "in file %s at line %d.\n",__FILE__,__LINE__);
		return;
	}

	if((prev = ptr->prev) != 0 && prev->isfree)
	{
		// the previous chunk is free, so
		// merge this chunk with the previous chunk
		prev->size += sizeof(struct MemEntry) + ptr->size;
		//begin add
		ptr->isfree=1;
		prev->succ = ptr->succ;
		if(ptr->succ != 0)
			ptr->succ->prev = prev;
		//end add
	}
	else
	{
		ptr->isfree = 1;
		prev = ptr;	// used for the step below
	}
	// printf("%d\n",ptr->succ->isfree);
	if((succ = ptr->succ) != 0 && succ->isfree)
	{
		// printf("Merging chunks of memory.\n");
		// the next chunk is free, merge with it
		prev->size += sizeof(struct MemEntry) + succ->size;
		prev->isfree = 1;
		//begin add
		prev->succ = succ->succ;
		if(succ->succ != 0)
			succ->succ->prev=prev;
		//end add
	}
}