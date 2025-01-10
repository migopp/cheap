#ifndef _CHEAP_ARENA
#define _CHEAP_ARENA
#include <stddef.h>

// Available allocators
typedef enum {
	CHEAP_BUDDY,
	CHEAP_BUMP,
	CHEAP_FL,
	CHEAP_POOL,
	CHEAP_STACK
} AllocatorType;

// Allocator object
typedef struct arena_allocator arena_allocator;

// CD
arena_allocator *arena_init(AllocatorType);
void arena_deinit(arena_allocator *);

// MMAN
void *arena_malloc(arena_allocator *, size_t);
void arena_free(arena_allocator *, void *);

// Debug
size_t arena_malloc_count(arena_allocator *);
size_t arena_free_count(arena_allocator *);
#endif	// !_CHEAP_ARENA
