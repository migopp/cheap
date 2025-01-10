#include "buddy.h"
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>

#define CHEAP_BUDDY_BLOCKS ((1 << (CHEAP_BUDDY_ORDERS + 1)) - 1)
#define CHEAP_BUDDY_LEAVES (1 << CHEAP_BUDDY_ORDERS)
#define CHEAP_BUDDY_TOTAL_SIZE (CHEAP_BUDDY_BLOCK_SIZE * CHEAP_BUDDY_LEAVES)

struct buddy_allocator {
	uint8_t buddy_md[CHEAP_BUDDY_BLOCKS];
	uint8_t *buddy_heap;
	size_t buddy_mallocc;
	size_t buddy_freec;
};

static size_t order(size_t size) {
	size_t ord = 0, factor = 1;
	while (size > (factor * CHEAP_BUDDY_BLOCK_SIZE)) {
		ord++;
		factor <<= 1;
	}

	return ord;
}

#define LEFT(parent) (2 * parent + 1)
#define RIGHT(parent) (2 * parent + 2)
#define PARENT(child) ((child - 1) / 2)
#define SIBLING(child) (((child - 1) ^ 1) + 1)

static void *alloc_order_h(buddy_allocator *a, uintptr_t l, uintptr_t r,
						   size_t idx, size_t c_ord, size_t t_ord) {
	// Base case
	if (t_ord == c_ord) {
		if (a->buddy_md[idx]) return NULL;
		a->buddy_md[idx] = 1;
		return (void *)l;
	}

	// Block is in use -- as is
	size_t l_idx = LEFT(idx);
	size_t r_idx = RIGHT(idx);
	if (a->buddy_md[idx] && !a->buddy_md[l_idx] && !a->buddy_md[r_idx])
		return NULL;

	// Try subtrees
	uintptr_t m = l + (r - l) / 2;
	void *left = alloc_order_h(a, l, m, l_idx, c_ord - 1, t_ord);
	if (left != NULL) {
		a->buddy_md[idx] = 1;
		return left;
	}
	void *right = alloc_order_h(a, m, r, r_idx, c_ord - 1, t_ord);
	if (right != NULL) {
		a->buddy_md[idx] = 1;
		return right;
	}

	// No space below
	return NULL;
}

// Allocates space for a given order
//
// The result will be the appropriate pointer, or NULL
// if there was an error.
static void *alloc_order(buddy_allocator *a, size_t t_ord) {
	uintptr_t heap_start = (uintptr_t)a->buddy_heap;
	uintptr_t heap_end = heap_start + CHEAP_BUDDY_TOTAL_SIZE;
	return alloc_order_h(a, heap_start, heap_end, 0, CHEAP_BUDDY_ORDERS, t_ord);
}

static size_t index_given_ptr(buddy_allocator *a, void *ptr) {
	// Get first index with matching ptr
	//
	// Binary search!
	uintptr_t p = (uintptr_t)ptr;
	uintptr_t l = (uintptr_t)a->buddy_heap;
	uintptr_t r = l + CHEAP_BUDDY_TOTAL_SIZE;
	size_t md = 0;
	while (l <= r) {
		if (l == p) break;
		uintptr_t m = l + (r - l) / 2;
		if (p == m) {
			md = RIGHT(md);
			l = m;
			break;
		} else if (p < m) {
			md = LEFT(md);
			r = m;
		} else {
			md = RIGHT(md);
			l = m;
		}
	}

	// Find deepest allocated node
	//
	// This will always be on the left
	size_t md_l = LEFT(md);
	while (md_l < CHEAP_BUDDY_BLOCKS && a->buddy_md[md_l]) {
		md = md_l;
		md_l = LEFT(md);
	}
	return md;
}

buddy_allocator *buddy_init(void) {
	// Make space for the actual allocator object
	buddy_allocator *a =
		mmap(NULL, sizeof(buddy_allocator), PROT_READ | PROT_WRITE,
			 MAP_ANON | MAP_PRIVATE, -1, 0);
	if (a == MAP_FAILED) {
		return NULL;
	}

	// Create heap
	//
	// Now, this is more of a job for `mmap`.
	uint8_t *h = mmap(NULL, CHEAP_BUDDY_TOTAL_SIZE, PROT_READ | PROT_WRITE,
					  MAP_ANON | MAP_PRIVATE, -1, 0);
	if (h == MAP_FAILED) {
		munmap(a, sizeof(buddy_allocator));
		return NULL;
	}

	// Init heap data
	a->buddy_heap = h;
	a->buddy_mallocc = 0;
	a->buddy_freec = 0;

	// Ensure buddy metadata is zero-initialized
	bzero(a->buddy_md, CHEAP_BUDDY_BLOCKS);

	return a;
}

void buddy_deinit(buddy_allocator *a) {
	if (!a) return;

	// Unmap our segments
	munmap(a->buddy_heap, CHEAP_BUDDY_TOTAL_SIZE);
	munmap(a, sizeof(buddy_allocator));
}

void *buddy_malloc(buddy_allocator *a, size_t size) {
	if (!a) return NULL;

	// Determine order
	size_t t_ord = order(size);
	if (t_ord > CHEAP_BUDDY_ORDERS) return NULL;

	// Find space in tree via metadata, then hand over
	void *ptr = alloc_order(a, t_ord);
	if (ptr != NULL) a->buddy_mallocc++;

	return ptr;
}

void buddy_free(buddy_allocator *a, void *ptr) {
	if (!a) return;
	if (!ptr) return;

	// Find metadata bits
	size_t idx = index_given_ptr(a, ptr);
	a->buddy_md[idx] = 0;

	// Adjust metadata
	size_t sib = SIBLING(idx);
	idx = PARENT(idx);
	while (idx < CHEAP_BUDDY_BLOCKS && sib < CHEAP_BUDDY_BLOCKS &&
		   !a->buddy_md[sib]) {
		a->buddy_md[idx] = 0;
		if (idx == 0) break;
		sib = SIBLING(idx);
		idx = PARENT(idx);
	}

	// Increment free count
	a->buddy_freec++;
}

size_t buddy_malloc_count(buddy_allocator *a) {
	if (!a) return 0;
	return a->buddy_mallocc;
}

size_t buddy_free_count(buddy_allocator *a) {
	if (!a) return 0;
	return a->buddy_freec;
}
