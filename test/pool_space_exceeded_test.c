#include "pool/pool.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_POOL
#error "POOL ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`pool_space_exceeded_test` init\n");
	pool_allocator a = pool_init();

	// Allocate
	uint8_t *f = (uint8_t *)pool_malloc(&a, 8193 * sizeof(uint8_t));
	ASSERT(f == NULL);

	// Match counts
	size_t pool_mallocc;
	if ((pool_mallocc = pool_malloc_count(&a)) != 0) {
		fprintf(stderr, "wrong `pool_mallocc`: %zu\n", pool_mallocc);
	}
	size_t pool_freec;
	if ((pool_freec = pool_free_count(&a)) != 0) {
		fprintf(stderr, "wrong `pool_freec`: %zu\n", pool_freec);
	}

	pool_deinit(&a);
	printf("`pool_space_exceeded_test` deinit\n");
}
