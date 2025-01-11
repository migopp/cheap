#include "fl.h"
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdbool.h>

// Each free list block has the following layout in memory:
//
//   8B   8B                 nB                8B
// ________________________________________________
// | sz | nx |              data             | he |
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

struct fl_allocator {
	uint8_t *fl_heap;
	fl_head_md *fl_first;
	size_t fl_mallocc;
	size_t fl_freec;
};

static size_t fl_frame_down(size_t addr) {
	return (addr / CHEAP_FL_FRAME_SIZE) * CHEAP_FL_FRAME_SIZE;
}

static bool fl_in_bounds_left(fl_allocator *f, void *p) {
	return p >= (void *)f->fl_heap;
}

static bool fl_in_bounds_right(fl_allocator *f, void *p) {
	return p < (void *)&f->fl_heap[CHEAP_FL_SIZE];
}

static bool fl_in_bounds(fl_allocator *f, void *p) {
	return fl_in_bounds_left(f, p) && fl_in_bounds_right(f, p);
}

static bool fl_is_free(fl_head_md *head) { return head->flb_size > 0; }

fl_allocator *fl_init(void) {
	// Make space for the actual allocator object
	fl_allocator *a = mmap(NULL, sizeof(fl_allocator), PROT_READ | PROT_WRITE,
						   MAP_ANON | MAP_PRIVATE, -1, 0);
	if (a == MAP_FAILED) {
		return NULL;
	}

	// Create heap
	uint8_t *h = mmap(NULL, CHEAP_FL_SIZE, PROT_READ | PROT_WRITE,
					  MAP_ANON | MAP_PRIVATE, -1, 0);
	if (h == MAP_FAILED) {
		munmap(a, sizeof(fl_allocator));
		return NULL;
	}

	// Init heap data
	a->fl_heap = h;
	a->fl_mallocc = 0;
	a->fl_freec = 0;

	// Head metadata
	fl_head_md *cheap_fl_head = (fl_head_md *)h;
	cheap_fl_head->flb_size = CHEAP_FL_SIZE - CHEAP_FL_MD_SIZE;
	cheap_fl_head->flb_next = NULL;

	// Tail metadata
	fl_tail_md *cheap_fl_tail =
		(fl_tail_md *)(&h[CHEAP_FL_SIZE] - sizeof(uintptr_t));
	cheap_fl_tail->flb_head = cheap_fl_head;

	// Init free list
	a->fl_first = cheap_fl_head;

	return a;
}

void fl_deinit(fl_allocator *a) {
	if (!a) return;

	// Unmap our segments
	munmap(a->fl_heap, CHEAP_FL_SIZE);
	munmap(a, sizeof(fl_allocator));
}

void *fl_malloc(fl_allocator *a, size_t size) {
	if (!a) return NULL;

	// Round up size to stay aligned
	if (size > SIZE_T_MAX - CHEAP_FL_FRAME_SIZE + 1) return NULL;
	size = fl_frame_down(size + CHEAP_FL_FRAME_SIZE - 1);

	// Nothing in free list
	if (a->fl_first == NULL) return NULL;

	// First fit
	fl_head_md *h = a->fl_first, *p = NULL;
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
		a->fl_first = fit->flb_next;
	} else {
		// This block was not first in the free list,
		// so connect previous block to the next
		p->flb_next = fit->flb_next;
	}

	// Set metadata
	fit->flb_size = -fit->flb_size;
	fit->flb_next = 0;

	// Increment malloc count and hand over
	a->fl_mallocc++;
	return (void *)(fit + 1);
}

void fl_free(fl_allocator *a, void *ptr) {
	if (!a) return;
	if (!ptr) return;

	// Validate
	uint8_t *n_ptr = (uint8_t *)ptr;
	fl_head_md *head = (fl_head_md *)(n_ptr - sizeof(fl_head_md));
	if (!fl_in_bounds(a, n_ptr) || !fl_in_bounds(a, head) || fl_is_free(head))
		return;

	// Mark as free
	head->flb_size = -head->flb_size;

	// Left coalesce
	fl_tail_md *left_tail =
		(fl_tail_md *)((uint8_t *)head - sizeof(fl_tail_md));
	if (fl_in_bounds_left(a, left_tail) &&
		fl_in_bounds_left(a, left_tail->flb_head) &&
		fl_is_free(left_tail->flb_head)) {
		fl_head_md *left_head = left_tail->flb_head;
		// Merge left
		left_head->flb_size += head->flb_size + CHEAP_FL_MD_SIZE;
		head = left_head;
		fl_tail_md *n_tail = (fl_tail_md *)((uint8_t *)head + head->flb_size +
											sizeof(fl_head_md));
		n_tail->flb_head = head;
	}

	// Right coalesce
	fl_head_md *right_head =
		(fl_head_md *)((uint8_t *)head + head->flb_size + CHEAP_FL_MD_SIZE);
	if (fl_in_bounds_right(a, right_head) && fl_is_free(right_head)) {
		// Merge right
		head->flb_size += right_head->flb_size + CHEAP_FL_MD_SIZE;
		fl_tail_md *n_tail = (fl_tail_md *)((uint8_t *)head + head->flb_size +
											sizeof(fl_head_md));
		n_tail->flb_head = head;
	}

	// Append to the free list
	fl_head_md *o_first = a->fl_first;
	if (o_first != NULL) {
		head->flb_next = o_first;
	}
	a->fl_first = head;

	// Increment free count
	a->fl_freec++;
}

size_t fl_malloc_count(fl_allocator *a) {
	if (!a) return 0;
	return a->fl_mallocc;
}

size_t fl_free_count(fl_allocator *a) {
	if (!a) return 0;
	return a->fl_freec;
}
