#ifndef _CHEAP_POOL
#define _CHEAP_POOL
#include <stddef.h>

// Heap size, in bytes
#define CHEAP_POOL_SIZE ((1 << 20) / 3)

#define CHEAP_POOL_S (2 << 4)
#define CHEAP_POOL_M (2 << 8)
#define CHEAP_POOL_L (2 << 12)

void pool_init(void);
void *pool_malloc(size_t);
void pool_free(void *);
size_t get_pool_mallocc(void);
size_t get_pool_freec(void);
#endif	// !_CHEAP_POOL
