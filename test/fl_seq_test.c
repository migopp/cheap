#include "fl/fl.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_FL
#error "FREE LIST ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`fl_seq_test` init\n");
	fl_allocator a = fl_init();

	// Allocate
	uint8_t *f = (uint8_t *)fl_malloc(&a, sizeof(uint8_t));
	ASSERT(f != NULL);
	uint8_t *n = (uint8_t *)fl_malloc(&a, sizeof(uint8_t));
	ASSERT(n != NULL);

	// Free our blocks
	fl_free(&a, f);
	fl_free(&a, n);

	// Match counts
	size_t fl_mallocc, fl_freec;
	if ((fl_mallocc = fl_malloc_count(&a)) != 2) {
		fprintf(stderr, "wrong `fl_mallocc`: %zu\n", fl_mallocc);
	}
	if ((fl_freec = fl_free_count(&a)) != 2) {
		fprintf(stderr, "wrong `fl_freec`: %zu\n", fl_freec);
	}

	fl_deinit(&a);
	printf("`fl_seq_test` deinit\n");
}
