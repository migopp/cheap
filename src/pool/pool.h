#ifndef _CHEAP_POOL
#define _CHEAP_POOL
#include <stdint.h>
#include <stddef.h>

// Settings
#define CHEAP_POOL_SIZE (1 << 18)
#define CHEAP_POOL_S (2 << 4)
#define CHEAP_POOL_M (2 << 8)
#define CHEAP_POOL_L (2 << 12)

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
typedef struct pfl_head_md {
	size_t pflb_size;
	struct pfl_head_md *pflb_next;
} pfl_head_md;

typedef struct pool_allocator {
	uint8_t *pool_sheap;
	uint8_t *pool_mheap;
	uint8_t *pool_lheap;
	pfl_head_md *pool_sfirst;
	pfl_head_md *pool_mfirst;
	pfl_head_md *pool_lfirst;
	size_t pool_mallocc;
	size_t pool_freec;
	uint8_t pool_valid;
} pool_allocator;

typedef enum { SMALL, MEDIUM, LARGE, NO_FIT } PoolSize;

pool_allocator pool_init(void);
void pool_deinit(pool_allocator *);

void *pool_malloc(pool_allocator *, size_t);
void pool_free(pool_allocator *, void *);

size_t pool_malloc_count(pool_allocator *);
size_t pool_free_count(pool_allocator *);
#endif	// !_CHEAP_POOL
