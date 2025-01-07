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
typedef struct flb_head_md {
	size_t flb_size;
	struct flb_head_md *flb_next;
} flb_head_md;

flb_head_md *s_first_flb = NULL;
flb_head_md *m_first_flb = NULL;
flb_head_md *l_first_flb = NULL;

void pool_init() {}

void *pool_malloc(size_t size) {
	(void)size;
	return NULL;
}

void pool_free(void *ptr) { (void)ptr; }

size_t get_pool_mallocc() { return pool_mallocc; }

size_t get_pool_freec() { return pool_freec; }
