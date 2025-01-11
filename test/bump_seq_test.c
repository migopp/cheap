#include "bump/bump.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_BUMP
#error "BUMP ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`bump_seq_test` init\n");
	bump_allocator a = bump_init();

	// Allocate
	uint8_t *f = (uint8_t *)bump_malloc(&a, sizeof(uint8_t));
	ASSERT(f != NULL);
	uint8_t *n = (uint8_t *)bump_malloc(&a, sizeof(uint8_t));
	ASSERT(n != NULL);

	// Match counts
	size_t bump_mallocc;
	if ((bump_mallocc = bump_malloc_count(&a)) != 2) {
		fprintf(stderr, "wrong `bump_mallocc`: %zu\n", bump_mallocc);
	}

	bump_deinit(&a);
	printf("`bump_seq_test` deinit\n");
}
