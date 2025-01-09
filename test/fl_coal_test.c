#include "fl/fl.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_FL
#error "FREE LIST ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`fl_coal_test` init\n");
	fl_allocator *a = fl_init();

	// Allocate the entire heap
	//
	// This should leave no room remaining
	uint8_t *f = (uint8_t *)fl_malloc(a, ((1 << 19) - 24) * sizeof(uint8_t));
	ASSERT(f != NULL);
	uint8_t *n = (uint8_t *)fl_malloc(a, ((1 << 19) - 24) * sizeof(uint8_t));
	ASSERT(n != NULL);

	// Now, if we allocate again it should fail
	uint8_t *fail = (uint8_t *)fl_malloc(a, sizeof(uint8_t));
	ASSERT(fail == NULL);

	// If we free those two giant blocks, they should
	// coalesce into one so as to reduce fragmentation
	fl_free(a, f);
	fl_free(a, n);
	n = (uint8_t *)fl_malloc(a, ((1 << 20) - 24) * sizeof(uint8_t));
	ASSERT(n != NULL);
	ASSERT(f == n);

	// Match counts
	size_t fl_mallocc, fl_freec;
	if ((fl_mallocc = fl_malloc_count(a)) != 3) {
		fprintf(stderr, "wrong `fl_mallocc`: %zu\n", fl_mallocc);
	}
	if ((fl_freec = fl_free_count(a)) != 2) {
		fprintf(stderr, "wrong `fl_freec`: %zu\n", fl_freec);
	}

	fl_deinit(a);
	printf("`fl_coal_test` deinit\n");
}
