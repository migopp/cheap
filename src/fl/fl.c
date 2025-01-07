#include "fl.h"
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

// TODO: REMOVE
#include <stdio.h>

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
typedef struct fl_head_md {
	ssize_t flb_size;			  // sz
	struct fl_head_md *flb_next;  // nx
} fl_head_md;
typedef struct fl_tail_md {
	fl_head_md *flb_head;  // he
} fl_tail_md;

const size_t CHEAP_FL_MD_SIZE = sizeof(fl_head_md) + sizeof(fl_tail_md);
fl_head_md *first_flb = NULL;

size_t fl_frame_down(size_t addr) {
	return (addr / CHEAP_FL_FRAME_SIZE) * CHEAP_FL_FRAME_SIZE;
}

size_t fl_frame_up(size_t addr) {
	return fl_frame_down(addr + CHEAP_FL_FRAME_SIZE - 1);
}

void fl_init() {
	// Head metadata
	fl_head_md *cheap_fl_head = (fl_head_md *)cheap_fl;
	cheap_fl_head->flb_size = CHEAP_FL_SIZE - CHEAP_FL_MD_SIZE;
	cheap_fl_head->flb_next = NULL;

	// Tail metadata
	fl_tail_md *cheap_fl_tail =
		(fl_tail_md *)(&cheap_fl[CHEAP_FL_SIZE] - sizeof(uintptr_t));
	cheap_fl_tail->flb_head = cheap_fl_head;

	// Init free list
	first_flb = cheap_fl_head;
}

void *fl_malloc(size_t size) {
	// Align size to 8B
	size = fl_frame_up(size);

	// Nothing in free list
	if (first_flb == NULL) return NULL;

	// First fit
	fl_head_md *h = first_flb, *p = NULL;
	while (h != NULL && h->flb_size > 0 && (size_t)h->flb_size < size) {
		p = h;
		h = h->flb_next;
	}
	if (h == NULL) return NULL;

	// Try to split
	fl_head_md *fit = h;
	ssize_t split_size = fit->flb_size - size - CHEAP_FL_MD_SIZE;
	if (split_size >= CHEAP_FL_SPLIT_MIN) {
		// Get split locations
		printf("size = %zu\n", size);
		printf("md size = %zu\n", CHEAP_FL_MD_SIZE);
		uint8_t *b_fit = (uint8_t *)fit;
		printf("b_fit = %p\n", b_fit);
		uint8_t *this_end = b_fit + sizeof(fl_head_md) + size;
		printf("this_end = %p\n", this_end);
		uint8_t *split_start = this_end + sizeof(fl_tail_md);
		printf("split_start = %p\n", split_start);
		uint8_t *split_end = b_fit + sizeof(fl_head_md) + fit->flb_size;
		printf("split_end = %p\n", split_end);

		// Set metadata for split block
		fl_head_md *split_start_md = (fl_head_md *)split_start;
		fl_tail_md *split_end_md = (fl_tail_md *)split_end;
		split_start_md->flb_size = split_size;
		split_start_md->flb_next = fit->flb_next;
		split_end_md->flb_head = split_start_md;

		// Set metadata for return block
		fl_tail_md *this_end_md = (fl_tail_md *)this_end;
		fit->flb_size = size;
		this_end_md->flb_head = fit;
	}

	// Set previous flb metadata
	if (p == NULL) {
		// This block was first in the free list
		first_flb = fit->flb_next;
	} else {
		// This block was not first in the free list,
		// so connect previous block to the next
		p->flb_next = fit->flb_next;
	}

	// Set our metadata and hand over
	fit->flb_size = -fit->flb_size;
	fit->flb_next = 0;
	return (void *)(fit + sizeof(fl_head_md));
}

void fl_free(void *ptr) {
	// Validate
	uint8_t *n_ptr = (uint8_t *)ptr;
	if (n_ptr < cheap_fl || n_ptr >= &cheap_fl[CHEAP_FL_SIZE]) return;
	// TODO:
	// 		- Check if metadata is in heap range
	// 		- Ensure that size is negative

	// Check for empty free list
	fl_head_md *o_first_flb = first_flb;
	fl_head_md *n_first_flb = (fl_head_md *)(n_ptr - sizeof(fl_head_md));
	if (o_first_flb == NULL) {
		first_flb = n_first_flb;
		return;
	}

	// TODO: Check for coalesce

	// Append to the free list
	n_first_flb->flb_next = o_first_flb;
	first_flb = n_first_flb;
}

size_t get_fl_mallocc() { return cheap_fl_mallocc; }

size_t get_fl_freec() { return cheap_fl_freec; }
