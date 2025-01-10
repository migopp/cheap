#include "arena/arena.h"
#include "buddy/buddy.h"
#include "bump/bump.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_ARENA
#error "ARENA ALLOCATOR NOT DEFINED"
#endif

#ifndef _CHEAP_BUDDY
#error "BUDDY ALLOCATOR NOT DEFINED"
#endif

#ifndef _CHEAP_BUMP
#error "BUMP ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`arena_test` init\n");
	arena_allocator *buddy = arena_init(CHEAP_BUDDY);
	ASSERT(buddy != NULL);
	arena_allocator *bump = arena_init(CHEAP_BUMP);
	ASSERT(bump != NULL);

	// Fill up `buddy`
	const size_t CHEAP_BUDDY_TOTAL_SIZE =
		CHEAP_BUDDY_BLOCK_SIZE * (1 << CHEAP_BUDDY_ORDERS);
	uint8_t *f = (uint8_t *)arena_malloc(buddy, CHEAP_BUDDY_TOTAL_SIZE);
	ASSERT(f != NULL);

	// Allocate a bit on `bump`
	uint8_t *n = (uint8_t *)arena_malloc(bump, CHEAP_BUMP_SIZE);
	ASSERT(n != NULL);

	// Free on `buddy`
	arena_free(buddy, f);
	// Then realloc
	uint8_t *m = (uint8_t *)arena_malloc(buddy, CHEAP_BUDDY_TOTAL_SIZE);
	ASSERT(f == m);

	// And on `bump`...
	// But, here it doesn't do anything!
	arena_free(bump, n);
	uint8_t *o = (uint8_t *)arena_malloc(bump, CHEAP_BUMP_SIZE);
	ASSERT(o == NULL);

	// Match counts
	size_t buddy_mallocc;
	if ((buddy_mallocc = arena_malloc_count(buddy)) != 2) {
		fprintf(stderr, "wrong `buddy_mallocc`: %zu\n", buddy_mallocc);
	}
	size_t buddy_freec;
	if ((buddy_freec = arena_free_count(buddy)) != 1) {
		fprintf(stderr, "wrong `buddy_freec`: %zu\n", buddy_freec);
	}
	size_t bump_mallocc;
	if ((bump_mallocc = arena_malloc_count(bump)) != 1) {
		fprintf(stderr, "wrong `bump_mallocc`: %zu\n", bump_mallocc);
	}
	size_t bump_freec;
	if ((bump_freec = arena_free_count(bump)) != 0) {
		fprintf(stderr, "wrong `bump_freec`: %zu\n", bump_freec);
	}

	arena_deinit(buddy);
	arena_deinit(bump);
	printf("`arena_test` deinit\n");
}
