#ifndef _CHEAP_BUMP
#define _CHEAP_BUMP
#include <stddef.h>

// Settings
#define CHEAP_BUMP_SIZE (1 << 20)

typedef struct bump_allocator bump_allocator;

bump_allocator *bump_init(void);
void bump_deinit(bump_allocator *);

void *bump_malloc(bump_allocator *, size_t);

size_t bump_malloc_count(bump_allocator *);
#endif	// !_CHEAP_BUMP
