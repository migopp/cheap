#ifndef _CHEAP_BUDDY
#define _CHEAP_BUDDY
#include <stdint.h>
#include <stddef.h>

// Settings
#define CHEAP_BUDDY_BLOCK_SIZE (1 << 4)
#define CHEAP_BUDDY_ORDERS 10

#define CHEAP_BUDDY_BLOCKS ((1 << (CHEAP_BUDDY_ORDERS + 1)) - 1)
typedef struct buddy_allocator {
	uint8_t buddy_md[CHEAP_BUDDY_BLOCKS];
	uint8_t *buddy_heap;
	size_t buddy_mallocc;
	size_t buddy_freec;
	uint8_t buddy_valid;
} buddy_allocator;

buddy_allocator buddy_init(void);
void buddy_deinit(buddy_allocator *);

void *buddy_malloc(buddy_allocator *, size_t);
void buddy_free(buddy_allocator *, void *);

size_t buddy_malloc_count(buddy_allocator *);
size_t buddy_free_count(buddy_allocator *);
#endif	// !_CHEAP_BUDDY
