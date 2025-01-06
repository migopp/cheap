#include "bump/bump.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_BUMP
#error "BUMP ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`bump_space_exceeded_test` init\n");

	// Fail to allocate!
	//
	// We should not have enough room.
	// 256 * 8 is like 2048, and
	// 2048 is only... 1 << 11?
	ASSERT(256 * 8 == 2048);
	ASSERT(2048 == (1 << 11));
	uint8_t *p = (uint8_t *)bump_malloc((1 << 11) * sizeof(uint8_t));
	if (p != NULL) {
		fprintf(stderr, "allocation succeeded when it was meant to fail: %p\n",
				p);
	}

	// Match counts
	size_t bump_mallocc;
	if ((bump_mallocc = get_bump_mallocc()) != 0) {
		fprintf(stderr, "wrong `bump_mallocc`: %zu\n", bump_mallocc);
	}

	printf("`bump_space_exceeded_test` deinit\n");
}
