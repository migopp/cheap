#include "fl/fl.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_FL
#error "FREE LIST ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`fl_seq_test` init\n");
	fl_init();

	// Allocate
	uint8_t *f = (uint8_t *)fl_malloc(sizeof(uint8_t));
	ASSERT(f != NULL);
	uint8_t *n = (uint8_t *)fl_malloc(sizeof(uint8_t));
	ASSERT(n != NULL);

	// Free our blocks
	fl_free(f);
	fl_free(n);

	// Match counts
	size_t fl_mallocc, fl_freec;
	if ((fl_mallocc = get_fl_mallocc()) != 2) {
		fprintf(stderr, "wrong `fl_mallocc`: %zu\n", fl_mallocc);
	}
	if ((fl_freec = get_fl_freec()) != 2) {
		fprintf(stderr, "wrong `fl_freec`: %zu\n", fl_freec);
	}

	printf("`fl_seq_test` deinit\n");
}
