#include "pool.h"
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdbool.h>

static PoolSize determine_size(size_t size) {
	if (size <= CHEAP_POOL_S) {
		return SMALL;
	} else if (size <= CHEAP_POOL_M) {
		return MEDIUM;
	} else if (size <= CHEAP_POOL_L) {
		return LARGE;
	} else {
		return NO_FIT;
	}
}

pool_allocator pool_init(void) {
	// Make space for the actual allocator object
	pool_allocator a;
	a.pool_valid = 0;

	// Create heaps
	uint8_t *s = mmap(NULL, CHEAP_POOL_SIZE, PROT_READ | PROT_WRITE,
					  MAP_ANON | MAP_PRIVATE, -1, 0);
	if (s == MAP_FAILED) return a;
	uint8_t *m = mmap(NULL, CHEAP_POOL_SIZE, PROT_READ | PROT_WRITE,
					  MAP_ANON | MAP_PRIVATE, -1, 0);
	if (m == MAP_FAILED) {
		munmap(s, CHEAP_POOL_SIZE);
		return a;
	}
	uint8_t *l = mmap(NULL, CHEAP_POOL_SIZE, PROT_READ | PROT_WRITE,
					  MAP_ANON | MAP_PRIVATE, -1, 0);
	if (l == MAP_FAILED) {
		munmap(s, CHEAP_POOL_SIZE);
		munmap(m, CHEAP_POOL_SIZE);
		return a;
	}

	// Init heap data
	a.pool_valid = 1;
	a.pool_sheap = s;
	a.pool_mheap = m;
	a.pool_lheap = l;
	a.pool_mallocc = 0;
	a.pool_freec = 0;

	// Small free list init
	const size_t S_BLOCKS = CHEAP_POOL_SIZE / CHEAP_POOL_S;
	a.pool_sfirst = (pfl_head_md *)a.pool_sheap;
	a.pool_sfirst->pflb_size = CHEAP_POOL_S;
	pfl_head_md *p = a.pool_sfirst;
	for (size_t i = 1; i < S_BLOCKS; ++i) {
		pfl_head_md *h = (pfl_head_md *)&a.pool_sheap[i * CHEAP_POOL_S];
		h->pflb_size = CHEAP_POOL_S;
		p->pflb_next = h;
		p = h;
	}
	p->pflb_next = NULL;

	// Medium free list init
	const size_t M_BLOCKS = CHEAP_POOL_SIZE / CHEAP_POOL_M;
	a.pool_mfirst = (pfl_head_md *)a.pool_mheap;
	a.pool_mfirst->pflb_size = CHEAP_POOL_M;
	p = a.pool_mfirst;
	for (size_t i = 1; i < M_BLOCKS; ++i) {
		pfl_head_md *h = (pfl_head_md *)&a.pool_mheap[i * CHEAP_POOL_M];
		h->pflb_size = CHEAP_POOL_M;
		p->pflb_next = h;
		p = h;
	}
	p->pflb_next = NULL;

	// Large free list init
	const size_t L_BLOCKS = CHEAP_POOL_SIZE / CHEAP_POOL_L;
	a.pool_lfirst = (pfl_head_md *)a.pool_lheap;
	a.pool_lfirst->pflb_size = CHEAP_POOL_L;
	p = a.pool_lfirst;
	for (size_t i = 1; i < L_BLOCKS; ++i) {
		pfl_head_md *h = (pfl_head_md *)&a.pool_lheap[i * CHEAP_POOL_L];
		h->pflb_size = CHEAP_POOL_L;
		p->pflb_next = h;
		p = h;
	}
	p->pflb_next = NULL;

	return a;
}

void pool_deinit(pool_allocator *a) {
	if (!a || !a->pool_valid) return;

	// Unmap our segments
	munmap(a->pool_sheap, CHEAP_POOL_SIZE);
	munmap(a->pool_mheap, CHEAP_POOL_SIZE);
	munmap(a->pool_lheap, CHEAP_POOL_SIZE);
}

void *pool_malloc(pool_allocator *a, size_t size) {
	if (!a || !a->pool_valid) return NULL;

	// Find and grab block from correct free list
	pfl_head_md *h = NULL;
	PoolSize ps = determine_size(size);
	switch (ps) {
		case SMALL:
			if (a->pool_sfirst == NULL) return NULL;
			h = a->pool_sfirst;
			a->pool_sfirst = a->pool_sfirst->pflb_next;
			break;
		case MEDIUM:
			if (a->pool_mfirst == NULL) return NULL;
			h = a->pool_mfirst;
			a->pool_mfirst = a->pool_mfirst->pflb_next;
			break;
		case LARGE:
			if (a->pool_lfirst == NULL) return NULL;
			h = a->pool_lfirst;
			a->pool_lfirst = a->pool_lfirst->pflb_next;
			break;
		case NO_FIT:
			return NULL;
	}

	// Set metadata
	h->pflb_next = 0;

	// Increment malloc count and hand over
	a->pool_mallocc++;
	return (void *)(h + 1);
}

void pool_free(pool_allocator *a, void *ptr) {
	if (!a || !a->pool_valid) return;

	// Get metadata head
	uint8_t *n_ptr = (uint8_t *)ptr;
	pfl_head_md *h = (pfl_head_md *)(n_ptr - sizeof(pfl_head_md));

	// Add back to appropriate free list
	PoolSize ps = determine_size(h->pflb_size);
	switch (ps) {
		case SMALL:
			h->pflb_next = a->pool_sfirst;
			a->pool_sfirst = h;
			break;
		case MEDIUM:
			h->pflb_next = a->pool_mfirst;
			a->pool_mfirst = h;
			break;
		case LARGE:
			h->pflb_next = a->pool_lfirst;
			a->pool_lfirst = h;
			break;
		case NO_FIT:
			return;
	}

	// Increment free count
	a->pool_freec++;
}

size_t pool_malloc_count(pool_allocator *a) {
	if (!a || !a->pool_valid) return 0;
	return a->pool_mallocc;
}

size_t pool_free_count(pool_allocator *a) {
	if (!a || !a->pool_valid) return 0;
	return a->pool_freec;
}
