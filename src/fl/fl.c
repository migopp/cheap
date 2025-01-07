#include "fl.h"
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdbool.h>

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

bool fl_in_bounds_left(void *p) { return p >= (void *)cheap_fl; }

bool fl_in_bounds_right(void *p) {
	return p < (void *)&cheap_fl[CHEAP_FL_SIZE];
}

bool fl_in_bounds(void *p) {
	return fl_in_bounds_left(p) && fl_in_bounds_right(p);
}

bool fl_is_free(fl_head_md *head) { return head->flb_size > 0; }

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
		uint8_t *b_fit = (uint8_t *)fit;
		uint8_t *this_end = b_fit + sizeof(fl_head_md) + size;
		uint8_t *split_start = this_end + sizeof(fl_tail_md);
		uint8_t *split_end = b_fit + sizeof(fl_head_md) + fit->flb_size;

		// Set metadata for split block
		fl_head_md *split_start_md = (fl_head_md *)split_start;
		fl_tail_md *split_end_md = (fl_tail_md *)split_end;
		split_start_md->flb_size = split_size;
		split_start_md->flb_next = fit->flb_next;
		split_end_md->flb_head = split_start_md;

		// Set metadata for return block
		fl_tail_md *this_end_md = (fl_tail_md *)this_end;
		fit->flb_size = size;
		fit->flb_next = split_start_md;
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

	// Set metadata
	fit->flb_size = -fit->flb_size;
	fit->flb_next = 0;

	// Increment malloc count and hand over
	cheap_fl_mallocc++;
	return (void *)(fit + 1);
}

void fl_free(void *ptr) {
	// Validate
	uint8_t *n_ptr = (uint8_t *)ptr;
	if (!fl_in_bounds(n_ptr)) return;
	// TODO:
	// 		- Check if metadata is in heap range
	// 		- Ensure that size is negative

	fl_head_md *head = (fl_head_md *)(n_ptr - sizeof(fl_head_md));
	head->flb_size = -head->flb_size;

	// Check for coalesce
	fl_tail_md *left_tail =
		(fl_tail_md *)((uint8_t *)head - sizeof(fl_tail_md));
	if (fl_in_bounds_left(left_tail) &&
		fl_in_bounds_left(left_tail->flb_head) &&
		fl_is_free(left_tail->flb_head)) {
		fl_head_md *left_head = left_tail->flb_head;
		// Merge left
		left_head->flb_size += head->flb_size + CHEAP_FL_MD_SIZE;
		head = left_head;
		fl_tail_md *n_tail =
			(fl_tail_md *)(head + head->flb_size + sizeof(fl_head_md));
		n_tail->flb_head = head;
	}
	fl_head_md *right_head =
		(fl_head_md *)((uint8_t *)head + head->flb_size + CHEAP_FL_MD_SIZE);
	if (fl_in_bounds_right(right_head) && fl_is_free(right_head)) {
		// Merge right
		head->flb_size += right_head->flb_size + CHEAP_FL_MD_SIZE;
		fl_tail_md *n_tail =
			(fl_tail_md *)(head + head->flb_size + sizeof(fl_head_md));
		n_tail->flb_head = head;
	}

	// Append to the free list
	fl_head_md *o_first_flb = first_flb;
	if (o_first_flb != NULL) {
		head->flb_next = o_first_flb;
	}
	first_flb = head;

	// Increment free count
	cheap_fl_freec++;
}

size_t get_fl_mallocc() { return cheap_fl_mallocc; }

size_t get_fl_freec() { return cheap_fl_freec; }
