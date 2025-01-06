#include "fl/fl.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_FL
#error "FREE LIST ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`fl_coal_test` init\n");

	// Allocate
	uint8_t *f = (uint8_t *)fl_malloc(sizeof(uint8_t));
	ASSERT(f != NULL);
	uint8_t *n = (uint8_t *)fl_malloc(sizeof(uint8_t));
	ASSERT(n != NULL);
	// In this free list impl, successive allocations
	// should be neighboring. Let's test that.
	//
	// So there should be 1 byte between `f` and `n`.
	if (f != n - 1) {
		fprintf(stderr, "%p != %p - 1\n", f, n);
	}

	// Match counts
	size_t fl_mallocc;
	if ((fl_mallocc = get_fl_mallocc()) != 2) {
		fprintf(stderr, "wrong `fl_mallocc`: %zu\n", fl_mallocc);
	}

	printf("`fl_seq_test` deinit\n");
}
