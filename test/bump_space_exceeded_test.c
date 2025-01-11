#include "bump/bump.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_BUMP
#error "BUMP ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`bump_space_exceeded_test` init\n");
	bump_allocator a = bump_init();

	// Fail to allocate!
	//
	// We should not have enough room.
	uint8_t *p = (uint8_t *)bump_malloc(&a, CHEAP_BUMP_SIZE + 1);
	if (p != NULL) {
		fprintf(stderr, "allocation succeeded when it was meant to fail: %p\n",
				p);
	}

	// Match counts
	size_t bump_mallocc;
	if ((bump_mallocc = bump_malloc_count(&a)) != 0) {
		fprintf(stderr, "wrong `bump_mallocc`: %zu\n", bump_mallocc);
	}

	bump_deinit(&a);
	printf("`bump_space_exceeded_test` deinit\n");
}
