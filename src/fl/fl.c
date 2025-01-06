#include "fl.h"
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

uint8_t cheap_fl[CHEAP_FL_SIZE];
size_t cheap_fl_mallocc = 0;
size_t cheap_fl_freec = 0;

// Each free list block has the following layout in memory:
//
//   8B   8B                 nB                 8B
// ________________________________________________
// | sz | nx |              data              | he |
// ------------------------------------------------
//
// Where `sz`, `nx`, and `he` are metadata, and:
// 		- `sz` is the size of `data`,
// 		- `nx` is a pointer to the _next_ free block,
// 		- `he` is a pointer to the head of the block.
//
// `sz` is negative to indicate that a block is allocated.
typedef struct fl_block {
	ssize_t flb_size;	 // sz
	uintptr_t flb_next;	 // nx
	void *flb_data;		 // data
	uintptr_t flb_head;	 // he
} fl_block;

void fl_init() {
	*((ssize_t *)&cheap_fl) = CHEAP_FL_SIZE;
	// TODO: ? Why is this unreadable as fart
	// *((uintptr_t *)((char *)&cheap_fl)[sizeof(ssize_t)]) = 0;
}

void *fl_malloc(size_t size) {
	(void)size;
	return NULL;
}

void fl_free(void *ptr) { (void)ptr; }

size_t get_fl_mallocc() { return cheap_fl_mallocc; }

size_t get_fl_freec() { return cheap_fl_freec; }
