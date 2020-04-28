#include "memorymgr.h"
#include <stdlib.h>
#include <stdio.h>

int *first_block;
int *last_block;

void initmemory(int size){  // initialize a simulated heap
	int num_bytes = size + 12;
	num_bytes = next_multiple_of_eight(num_bytes);
	int *ptr = malloc(num_bytes);

	first_block = ptr + 1;
	*first_block = num_bytes - 8;
	last_block = first_block + (num_bytes/4)-2;
	*last_block = 1;
}

void *myalloc(int length){   // allocate a block of length (or more) bytes
	int *p = first_block;
	int size = next_multiple_of_eight(length+4);
	while(isAllocated(p) || (size>*p)){
		if (p==last_block) {return NULL;} 
		p = nextBlock(p);
	}

	int *next = p + size/4;
	if (*next!=1){
		*next = *p - size;
	}
	*p = size + 1;
	return p + 1;
} 

void myfree(void *ptr){     // free an allocated block
	int *p = ptr - 4;
	*p = *p - 1;
	return;
}

void coalesce(){	// go through the heap, coalescing unallocated blocks
	int *p = first_block;
	int size = *p;
	while (size!=1) {
		int *next = nextBlock(p);
		while (!isAllocated(p) && !isAllocated(next)){
			int next_size = *next;
			*p = size + next_size;
			next = nextBlock(next);
			size = *p;
		}
		p = nextBlock(p);
		size = *p;
	}
}

void printallocation(){  // print info about the simulated heap, for debugging purposes
/*
	int block_counter = 0;
	int size = *first_block;
	int *block = first_block;

	while (size != 1) {
		if (isAllocated(block))
			printf("Block %d: size %d \t allocated\n", block_counter, size - 1);
		else
			printf("Block %d: size %d \t unallocated\n", block_counter, size);
		block_counter++;
		block = nextBlock(block);
		size = *block;
	}
	printf("\n");
*/

	int *ptr = firstBlock();
	int count = 0;
	while (ptr != lastBlock()) {
		int length = (*ptr)/8*8;
		printf("Block %d: size %d \t", count, length);
		if (isAllocated(ptr)) 
			printf("allocated\n");
		else
			printf("unallocated\n");
		ptr = nextBlock(ptr);
		count++;
	}
	printf("\n");
}

int isAllocated(int *p){  // is the block at location p allocated?
	return (*p)%2==0?0:1;
}

int *nextBlock(int *p){  // return a pointer to the block that follows p
	return p + (*p / 4);
}

int *firstBlock(){         // return a pointer to the first block on the heap
	return first_block;
}

int *lastBlock(){   // return a pointer to the sentinel block
	return last_block;
}

int next_multiple_of_eight(int num){ //rounds up to the next multiple of 8
	while (num%8 != 0)
		num++;
	return num;
}
