// CHEAP_FL_SIZE: 1 << 20

#include "fl/fl.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_FL
#error "FREE LIST ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`fl_space_exceeded_test` init\n");

	// Fail to allocate!
	//
	// We should not have enough room.
	uint8_t *p = (uint8_t *)fl_malloc((1 << 21) * sizeof(uint8_t));
	ASSERT(p == NULL);

	// What about a _more_ clever failure?
	//
	// Fill up the heap.
	uint8_t *f = (uint8_t *)fl_malloc((1 << 18) * sizeof(uint8_t));
	ASSERT(f != NULL);
	uint8_t *n = (uint8_t *)fl_malloc((1 << 2) * sizeof(uint8_t));
	ASSERT(n != NULL);
	// Free end
	fl_free(n);
	// Allocate slightly more than the limit
	n = (uint8_t *)fl_malloc((1 << 3) * sizeof(uint8_t));
	ASSERT(n == NULL);
	// I never said it was _actually_ clever.

	// Match counts
	size_t fl_mallocc, fl_freec;
	if ((fl_mallocc = get_fl_mallocc()) != 2) {
		fprintf(stderr, "wrong `fl_mallocc`: %zu\n", fl_mallocc);
	}
	if ((fl_freec = get_fl_freec()) != 1) {
		fprintf(stderr, "wrong `fl_freec`: %zu\n", fl_freec);
	}

	printf("`fl_space_exceeded_test` deinit\n");
}
