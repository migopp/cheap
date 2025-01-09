#include "buddy.h"
#include "allocator.h"
#include <string.h>
#include <stdint.h>
#include <sys/mman.h>

#define CHEAP_BUDDY_BLOCKS ((1 << (CHEAP_BUDDY_ORDERS + 1)) - 1)
#define CHEAP_BUDDY_LEAVES (1 << CHEAP_BUDDY_ORDERS)
#define CHEAP_BUDDY_TOTAL_SIZE (CHEAP_BUDDY_BLOCK_SIZE * CHEAP_BUDDY_LEAVES)

struct buddy_allocator {
	AllocatorType a_type;
	uint8_t buddy_md[CHEAP_BUDDY_BLOCKS];
	uint8_t *buddy_heap;
	size_t buddy_mallocc;
	size_t buddy_freec;
};

size_t order(size_t size) {
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

ssize_t alloc_order(buddy_allocator *a, size_t idx, size_t t_ord,
					size_t c_ord) {
	if (t_ord == c_ord) {
		if (a->buddy_md[idx]) return -1;
		a->buddy_md[idx] = 1;
		return idx;
	}

	ssize_t left = alloc_order(a, LEFT(idx), t_ord, c_ord - 1);
	if (left != -1) {
		a->buddy_md[idx] = 1;
		return left;
	}

	ssize_t right = alloc_order(a, RIGHT(idx), t_ord, c_ord - 1);
	if (right != -1) {
		a->buddy_md[idx] = 1;
		return right;
	}

	return -1;
}

buddy_allocator *buddy_init(void) {
	// Make space for the actual allocator object
	//
	// Would use `sbrk` but its depricated? `mmap` seems like a
	// tad bit of a waste, since it'll give us a whole page,
	// but oh well.
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
	a->a_type = BUDDY;
	a->buddy_heap = h;
	a->buddy_mallocc = 0;
	a->buddy_freec = 0;

	// Ensure buddy metadata is zero initialized
	bzero(a->buddy_md, CHEAP_BUDDY_BLOCKS);

	return a;
}

void buddy_deinit(buddy_allocator *a) {
	// Unmap our segments
	munmap(a->buddy_heap, CHEAP_BUDDY_TOTAL_SIZE);
	munmap(a, sizeof(buddy_allocator));
}

void *buddy_malloc(buddy_allocator *a, size_t size) {
	// Determine order
	size_t t_ord = order(size);
	if (t_ord > CHEAP_BUDDY_ORDERS) return NULL;

	// Find space in tree via metadata
	ssize_t alloc_idx = alloc_order(a, 0, t_ord, CHEAP_BUDDY_ORDERS);
	if (alloc_idx == -1) return NULL;

	// Convert metadata index to heap ptr
	void *ptr = a->buddy_heap + alloc_idx * CHEAP_BUDDY_BLOCK_SIZE;

	// Increment malloc count and hand over
	a->buddy_mallocc++;
	return ptr;
}

void buddy_free(buddy_allocator *a, void *ptr) {
	// Round down to nearest block
	// (or assume for now it's an even block #)

	// Find metadata bits
	size_t offset = (uintptr_t)ptr - (uintptr_t)a->buddy_heap;
	size_t idx = offset / CHEAP_BUDDY_BLOCK_SIZE;
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

size_t buddy_malloc_count(buddy_allocator *a) { return a->buddy_mallocc; }

size_t buddy_free_count(buddy_allocator *a) { return a->buddy_freec; }
