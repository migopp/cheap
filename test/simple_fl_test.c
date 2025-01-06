#define CHEAP_FL_SIZE 1 << 5

#include "fl/fl.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_FL
#error "FREE LIST ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`simple_fl_test` init\n");

	// Allocate
	uint8_t *f = (uint8_t *)fl_malloc(sizeof(uint8_t));
	uint8_t *n = (uint8_t *)fl_malloc(sizeof(uint8_t));
	// In this free list impl, successive allocations
	// should be neighboring. Let's test that.
	//
	// So there should be 1 byte between `f` and `n`.
	if (/*f != n - 1*/ f != n) {
		fprintf(stderr, "%p != %p - 1\n", f, n);
	}

	// Match counts
	size_t fl_mallocc;
	if ((fl_mallocc = get_fl_mallocc()) != 2) {
		fprintf(stderr, "wrong `fl_mallocc`: %zu\n", fl_mallocc);
	}

	printf("`simple_fl_test` deinit\n");
}
