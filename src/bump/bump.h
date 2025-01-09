#ifndef _CHEAP_BUMP
#define _CHEAP_BUMP
#include <stddef.h>

// Settings
#define CHEAP_BUMP_SIZE (1 << 20)

// Allocator object
typedef struct bump_allocator bump_allocator;

// CD
bump_allocator *bump_init(void);
void bump_deinit(bump_allocator *);

// MMAN
void *bump_malloc(bump_allocator *, size_t);

// Debug
size_t bump_malloc_count(bump_allocator *);
#endif	// !_CHEAP_BUMP
