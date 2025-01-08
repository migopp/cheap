#include "pool.h"
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <stdbool.h>

uint8_t cheap_spool[CHEAP_POOL_SIZE];
uint8_t cheap_mpool[CHEAP_POOL_SIZE];
uint8_t cheap_lpool[CHEAP_POOL_SIZE];
size_t pool_mallocc = 0;
size_t pool_freec = 0;

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

fl_head_md *s_first_flb = NULL;
fl_head_md *m_first_flb = NULL;
fl_head_md *l_first_flb = NULL;

typedef enum PoolSize { SMALL, MEDIUM, LARGE, NO_FIT } PoolSize;

PoolSize determine_size(size_t size) {
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

void pool_init(void) {
	// Small
	const size_t S_BLOCKS = CHEAP_POOL_SIZE / CHEAP_POOL_S;
	s_first_flb = (fl_head_md *)cheap_spool;
	s_first_flb->flb_size = CHEAP_POOL_S;
	fl_head_md *p = s_first_flb;
	for (size_t i = 1; i < S_BLOCKS; ++i) {
		fl_head_md *h = (fl_head_md *)&cheap_spool[i * CHEAP_POOL_S];
		h->flb_size = CHEAP_POOL_S;
		p->flb_next = h;
		p = h;
	}
	p->flb_next = NULL;

	// Medium
	const size_t M_BLOCKS = CHEAP_POOL_SIZE / CHEAP_POOL_M;
	m_first_flb = (fl_head_md *)cheap_mpool;
	m_first_flb->flb_size = CHEAP_POOL_M;
	p = m_first_flb;
	for (size_t i = 1; i < M_BLOCKS; ++i) {
		fl_head_md *h = (fl_head_md *)&cheap_mpool[i * CHEAP_POOL_M];
		h->flb_size = CHEAP_POOL_M;
		p->flb_next = h;
		p = h;
	}
	p->flb_next = NULL;

	// Large
	const size_t L_BLOCKS = CHEAP_POOL_SIZE / CHEAP_POOL_L;
	l_first_flb = (fl_head_md *)cheap_lpool;
	l_first_flb->flb_size = CHEAP_POOL_L;
	p = l_first_flb;
	for (size_t i = 1; i < L_BLOCKS; ++i) {
		fl_head_md *h = (fl_head_md *)&cheap_lpool[i * CHEAP_POOL_L];
		h->flb_size = CHEAP_POOL_L;
		p->flb_next = h;
		p = h;
	}
	p->flb_next = NULL;
}

void *pool_malloc(size_t size) {
	// Find and grab block from correct free list
	fl_head_md *h = NULL;
	PoolSize ps = determine_size(size);
	switch (ps) {
		case SMALL:
			if (s_first_flb == NULL) return NULL;
			h = s_first_flb;
			s_first_flb = s_first_flb->flb_next;
			break;
		case MEDIUM:
			if (m_first_flb == NULL) return NULL;
			h = m_first_flb;
			m_first_flb = m_first_flb->flb_next;
			break;
		case LARGE:
			if (l_first_flb == NULL) return NULL;
			h = l_first_flb;
			l_first_flb = l_first_flb->flb_next;
			break;
		case NO_FIT:
			return NULL;
	}

	// Set metadata
	h->flb_next = 0;

	// Increment malloc count and hand over
	pool_mallocc++;
	return (void *)(h + 1);
}

void pool_free(void *ptr) {
	// Get metadata head
	uint8_t *n_ptr = (uint8_t *)ptr;
	fl_head_md *h = (fl_head_md *)(n_ptr - sizeof(fl_head_md));

	// Add back to appropriate free list
	PoolSize ps = determine_size(h->flb_size);
	switch (ps) {
		case SMALL:
			h->flb_next = s_first_flb;
			s_first_flb = h;
			break;
		case MEDIUM:
			h->flb_next = m_first_flb;
			m_first_flb = h;
			break;
		case LARGE:
			h->flb_next = l_first_flb;
			l_first_flb = h;
			break;
		case NO_FIT:
			return;
	}

	// Increment free count
	pool_freec++;
}

size_t get_pool_mallocc(void) { return pool_mallocc; }

size_t get_pool_freec(void) { return pool_freec; }
