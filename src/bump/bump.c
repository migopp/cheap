#include "bump.h"
#include "allocator.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>

struct bump_allocator {
	AllocatorType a_type;
	uint8_t *bmp_heap;
	size_t bmp_idx;
	size_t bmp_mallocc;
};

bump_allocator *bump_init(void) {
	// Make space for the actual allocator object
	//
	// Would use `sbrk` but its depricated? `mmap` seems like a
	// tad bit of a waste, since it'll give us a whole page,
	// but oh well.
	bump_allocator *a =
		mmap(NULL, sizeof(bump_allocator), PROT_READ | PROT_WRITE,
			 MAP_ANON | MAP_PRIVATE, -1, 0);
	if (a == MAP_FAILED) {
		return NULL;
	}

	// Create heap
	//
	// Now, this is more of a job for `mmap`.
	uint8_t *h = mmap(NULL, CHEAP_BUMP_SIZE, PROT_READ | PROT_WRITE,
					  MAP_ANON | MAP_PRIVATE, -1, 0);
	if (h == MAP_FAILED) {
		munmap(a, sizeof(bump_allocator));
		return NULL;
	}

	// Init heap data
	a->a_type = BUMP;
	a->bmp_heap = h;
	a->bmp_idx = 0;
	a->bmp_mallocc = 0;

	return a;
}

void bump_deinit(bump_allocator *a) {
	if (!a) return;

	// Unmap our segments
	munmap(a->bmp_heap, CHEAP_BUMP_SIZE);
	munmap(a, sizeof(bump_allocator));
}

void *bump_malloc(bump_allocator *a, size_t size) {
	if (!a) return NULL;

	// Alas, for the bump allocator this comes too soon!
	if (a->bmp_idx + size > CHEAP_BUMP_SIZE) return NULL;

	// Bump it.
	void *ptr = (void *)&a->bmp_heap[a->bmp_idx];
	a->bmp_idx += size;
	a->bmp_mallocc++;
	return ptr;
}

size_t bump_malloc_count(bump_allocator *a) {
	if (!a) return 0;
	return a->bmp_mallocc;
}
