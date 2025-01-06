#define CHEAP_BUMP_SIZE 1 << 5

#include "bump/bump.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_BUMP
#error "BUMP ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`simple_bump_test` init\n");

	// Allocate
	uint8_t *f = (uint8_t *)bump_malloc(sizeof(uint8_t));
	uint8_t *n = (uint8_t *)bump_malloc(sizeof(uint8_t));
	// If we bump, consecutive allocations should be
	// neighboring
	//
	// So there should be 1 byte between `f` and `n`
	if (f != n - 1) {
		fprintf(stderr, "%p != %p - 1\n", f, n);
	}

	// Match counts
	size_t bump_mallocc;
	if ((bump_mallocc = get_bump_mallocc()) != 2) {
		fprintf(stderr, "wrong `bump_mallocc`: %zu\n", bump_mallocc);
	}

	printf("`simple_bump_test` deinit\n");
}
