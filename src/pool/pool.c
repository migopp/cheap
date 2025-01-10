#include "pool.h"
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <stdbool.h>

// Each free list block has the following layout in memory:
//
//   8B   8B                 nB
// ________________________________________________
// | sz | nx |              data                  |
// ------------------------------------------------
//
// Where `sz` and `nx` are metadata, and:
// 		- `sz` is the size of `data`,
// 		- `nx` is a pointer to the _next_ free block.
typedef struct fl_head_md {
	size_t flb_size;
	struct fl_head_md *flb_next;
} fl_head_md;

struct pool_allocator {
	uint8_t *pool_sheap;
	uint8_t *pool_mheap;
	uint8_t *pool_lheap;
	fl_head_md *pool_sfirst;
	fl_head_md *pool_mfirst;
	fl_head_md *pool_lfirst;
	size_t pool_mallocc;
	size_t pool_freec;
};

typedef enum { SMALL, MEDIUM, LARGE, NO_FIT } PoolSize;

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

pool_allocator *pool_init(void) {
	// Make space for the actual allocator object
	pool_allocator *a =
		mmap(NULL, sizeof(pool_allocator), PROT_READ | PROT_WRITE,
			 MAP_ANON | MAP_PRIVATE, -1, 0);
	if (a == MAP_FAILED) {
		return NULL;
	}

	// Create heaps
	uint8_t *s = mmap(NULL, CHEAP_POOL_SIZE, PROT_READ | PROT_WRITE,
					  MAP_ANON | MAP_PRIVATE, -1, 0);
	if (s == MAP_FAILED) {
		munmap(a, sizeof(pool_allocator));
		return NULL;
	}
	uint8_t *m = mmap(NULL, CHEAP_POOL_SIZE, PROT_READ | PROT_WRITE,
					  MAP_ANON | MAP_PRIVATE, -1, 0);
	if (m == MAP_FAILED) {
		munmap(a, sizeof(pool_allocator));
		munmap(s, CHEAP_POOL_SIZE);
		return NULL;
	}
	uint8_t *l = mmap(NULL, CHEAP_POOL_SIZE, PROT_READ | PROT_WRITE,
					  MAP_ANON | MAP_PRIVATE, -1, 0);
	if (l == MAP_FAILED) {
		munmap(a, sizeof(pool_allocator));
		munmap(s, CHEAP_POOL_SIZE);
		munmap(m, CHEAP_POOL_SIZE);
		return NULL;
	}

	// Init heap data
	a->pool_sheap = s;
	a->pool_mheap = m;
	a->pool_lheap = l;
	a->pool_mallocc = 0;
	a->pool_freec = 0;

	// Small free list init
	const size_t S_BLOCKS = CHEAP_POOL_SIZE / CHEAP_POOL_S;
	a->pool_sfirst = (fl_head_md *)a->pool_sheap;
	a->pool_sfirst->flb_size = CHEAP_POOL_S;
	fl_head_md *p = a->pool_sfirst;
	for (size_t i = 1; i < S_BLOCKS; ++i) {
		fl_head_md *h = (fl_head_md *)&a->pool_sheap[i * CHEAP_POOL_S];
		h->flb_size = CHEAP_POOL_S;
		p->flb_next = h;
		p = h;
	}
	p->flb_next = NULL;

	// Medium free list init
	const size_t M_BLOCKS = CHEAP_POOL_SIZE / CHEAP_POOL_M;
	a->pool_mfirst = (fl_head_md *)a->pool_mheap;
	a->pool_mfirst->flb_size = CHEAP_POOL_M;
	p = a->pool_mfirst;
	for (size_t i = 1; i < M_BLOCKS; ++i) {
		fl_head_md *h = (fl_head_md *)&a->pool_mheap[i * CHEAP_POOL_M];
		h->flb_size = CHEAP_POOL_M;
		p->flb_next = h;
		p = h;
	}
	p->flb_next = NULL;

	// Large free list init
	const size_t L_BLOCKS = CHEAP_POOL_SIZE / CHEAP_POOL_L;
	a->pool_lfirst = (fl_head_md *)a->pool_lheap;
	a->pool_lfirst->flb_size = CHEAP_POOL_L;
	p = a->pool_lfirst;
	for (size_t i = 1; i < L_BLOCKS; ++i) {
		fl_head_md *h = (fl_head_md *)&a->pool_lheap[i * CHEAP_POOL_L];
		h->flb_size = CHEAP_POOL_L;
		p->flb_next = h;
		p = h;
	}
	p->flb_next = NULL;

	return a;
}

void pool_deinit(pool_allocator *a) {
	if (!a) return;

	// Unmap our segments
	munmap(a->pool_sheap, CHEAP_POOL_SIZE);
	munmap(a->pool_mheap, CHEAP_POOL_SIZE);
	munmap(a->pool_lheap, CHEAP_POOL_SIZE);
	munmap(a, sizeof(pool_allocator));
}

void *pool_malloc(pool_allocator *a, size_t size) {
	if (!a) return NULL;

	// Find and grab block from correct free list
	fl_head_md *h = NULL;
	PoolSize ps = determine_size(size);
	switch (ps) {
		case SMALL:
			if (a->pool_sfirst == NULL) return NULL;
			h = a->pool_sfirst;
			a->pool_sfirst = a->pool_sfirst->flb_next;
			break;
		case MEDIUM:
			if (a->pool_mfirst == NULL) return NULL;
			h = a->pool_mfirst;
			a->pool_mfirst = a->pool_mfirst->flb_next;
			break;
		case LARGE:
			if (a->pool_lfirst == NULL) return NULL;
			h = a->pool_lfirst;
			a->pool_lfirst = a->pool_lfirst->flb_next;
			break;
		case NO_FIT:
			return NULL;
	}

	// Set metadata
	h->flb_next = 0;

	// Increment malloc count and hand over
	a->pool_mallocc++;
	return (void *)(h + 1);
}

void pool_free(pool_allocator *a, void *ptr) {
	if (!a) return;

	// Get metadata head
	uint8_t *n_ptr = (uint8_t *)ptr;
	fl_head_md *h = (fl_head_md *)(n_ptr - sizeof(fl_head_md));

	// Add back to appropriate free list
	PoolSize ps = determine_size(h->flb_size);
	switch (ps) {
		case SMALL:
			h->flb_next = a->pool_sfirst;
			a->pool_sfirst = h;
			break;
		case MEDIUM:
			h->flb_next = a->pool_mfirst;
			a->pool_mfirst = h;
			break;
		case LARGE:
			h->flb_next = a->pool_lfirst;
			a->pool_lfirst = h;
			break;
		case NO_FIT:
			return;
	}

	// Increment free count
	a->pool_freec++;
}

size_t pool_malloc_count(pool_allocator *a) {
	if (!a) return 0;
	return a->pool_mallocc;
}

size_t pool_free_count(pool_allocator *a) {
	if (!a) return 0;
	return a->pool_freec;
}
