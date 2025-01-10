#ifndef _CHEAP_POOL
#define _CHEAP_POOL
#include <stddef.h>

// Settings
#define CHEAP_POOL_SIZE (1 << 18)
#define CHEAP_POOL_S (2 << 4)
#define CHEAP_POOL_M (2 << 8)
#define CHEAP_POOL_L (2 << 12)

typedef struct pool_allocator pool_allocator;

pool_allocator *pool_init(void);
void pool_deinit(pool_allocator *);

void *pool_malloc(pool_allocator *, size_t);
void pool_free(pool_allocator *, void *);

size_t pool_malloc_count(pool_allocator *);
size_t pool_free_count(pool_allocator *);
#endif	// !_CHEAP_POOL
