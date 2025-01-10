#ifndef _CHEAP_BUDDY
#define _CHEAP_BUDDY
#include <stddef.h>

// Settings
#define CHEAP_BUDDY_BLOCK_SIZE (1 << 4)
#define CHEAP_BUDDY_ORDERS 10

// Allocator object
typedef struct buddy_allocator buddy_allocator;

// CD
buddy_allocator *buddy_init(void);
void buddy_deinit(buddy_allocator *);

// MMAN
void *buddy_malloc(buddy_allocator *, size_t);
void buddy_free(buddy_allocator *, void *);

// Debug
size_t buddy_malloc_count(buddy_allocator *);
size_t buddy_free_count(buddy_allocator *);
#endif	// !_CHEAP_BUDDY
