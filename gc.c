#include "gc.h"
#include <stdio.h>
#include <stdlib.h>

void mark(void *p);
void sweep();
void *stackBottom();
int  pointsToHeap(int *p);
int  *findBlock(int *ptr);

void gc() { // perform garbage collection on the heap
	int **max = (int**)stackBottom();	
	int *dummy;
	int **p = &dummy; //creates pointer to top of stack

	while (p <= max) { //iterates through the stack
		mark(*p);
		p++;
	}
	sweep();
	coalesce();
}

void mark(void *p) {
	int *ptr = (int *) p;
	if (!pointsToHeap(ptr)) {return;}
	
	int *block = findBlock(ptr); //finds which block the pointer is pointing to

	if (isAllocated(block)) {
		*block = *block + 2; //mark the block by changing the second to last bit
		int size = *(block) / 4;
		int **subp = (int**)(block + 1);
		while (subp <= (int**)(block + size)) {//looks to see if there are any pointers in the block that also point to the heap
			mark(*subp);
			subp++;
		}
	}
}

void sweep() {
	int *ptr = (int*)firstBlock();
	int *sentinel = (int*)lastBlock();
	while (ptr!=sentinel) {
		if (isAllocated(ptr))
			if ((*ptr%4)!=3)
				myfree(ptr+1); //frees garbage
			else //unmarks the block
				*ptr = *ptr - 2;
		ptr = (int*)nextBlock(ptr);
	}
	return;
}

void *stackBottom() {
	unsigned long bottom;
    FILE *statfp = fopen("/proc/self/stat", "r");
    fscanf(statfp,
           "%*d %*s %*c %*d %*d %*d %*d %*d %*u "
           "%*u %*u %*u %*u %*u %*u %*d %*d "
           "%*d %*d %*d %*d %*u %*u %*d "
           "%*u %*u %*u %lu", &bottom);
    fclose(statfp);
	return (void *) bottom;
}

int pointsToHeap(int *p) { //determines if the pointer points between the bounds of the heap
	return (p >= (int*)firstBlock() && p < (int*)lastBlock());
}

int *findBlock(int *ptr) { //determines which block of the heap the pointer is pointing to
	int *block = (int*)firstBlock();

	while ((int*)nextBlock(block) < ptr) {
		block = (int*)nextBlock(block);
	}
	return block;	
}

