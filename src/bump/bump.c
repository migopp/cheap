#include "bump.h"
#include <limits.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <sys/mman.h>

static size_t bump_frame_down(size_t addr) {
	return (addr / CHEAP_BUMP_FRAME_SIZE) * CHEAP_BUMP_FRAME_SIZE;
}

bump_allocator bump_init(void) {
	// Make space for the actual allocator object
	bump_allocator a;
	a.bump_valid = 0;

	// Create heap
	uint8_t *h = mmap(NULL, CHEAP_BUMP_SIZE, PROT_READ | PROT_WRITE,
					  MAP_ANON | MAP_PRIVATE, -1, 0);
	if (h == MAP_FAILED) {
		return a;
	}

	// Init heap data
	a.bump_valid = 1;
	a.bmp_heap = h;
	a.bmp_idx = 0;
	a.bmp_mallocc = 0;

	return a;
}

void bump_deinit(bump_allocator *a) {
	if (!a || !a->bump_valid) return;

	// Unmap our segments
	munmap(a->bmp_heap, CHEAP_BUMP_SIZE);
}

void *bump_malloc(bump_allocator *a, size_t size) {
	if (!a || !a->bump_valid) return NULL;

	// Round up size to stay aligned
	if (size > SIZE_MAX - CHEAP_BUMP_FRAME_SIZE + 1) return NULL;
	size = bump_frame_down(size + CHEAP_BUMP_FRAME_SIZE - 1);

	// Alas, for the bump allocator this comes too soon!
	//
	// But, first ensure the new idx doesn't overflow
	if (a->bmp_idx <= SIZE_MAX - size && a->bmp_idx + size > CHEAP_BUMP_SIZE)
		return NULL;

	// Bump it.
	void *ptr = (void *)&a->bmp_heap[a->bmp_idx];
	a->bmp_idx += size;
	a->bmp_mallocc++;
	return ptr;
}

size_t bump_malloc_count(bump_allocator *a) {
	if (!a || !a->bump_valid) return 0;
	return a->bmp_mallocc;
}
