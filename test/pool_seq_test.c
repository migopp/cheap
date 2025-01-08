#include "pool/pool.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_POOL
#error "POOL ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`pool_seq_test` init\n");
	pool_init();

	// Allocate
	uint8_t *f = (uint8_t *)pool_malloc(sizeof(uint8_t));
	ASSERT(f != NULL);
	uint8_t *n = (uint8_t *)pool_malloc(sizeof(uint8_t));
	ASSERT(n != NULL);

	// Free our blocks
	pool_free(f);
	pool_free(n);

	// Match counts
	size_t pool_mallocc;
	if ((pool_mallocc = get_pool_mallocc()) != 2) {
		fprintf(stderr, "wrong `pool_mallocc`: %zu\n", pool_mallocc);
	}
	size_t pool_freec;
	if ((pool_freec = get_pool_freec()) != 2) {
		fprintf(stderr, "wrong `pool_freec`: %zu\n", pool_freec);
	}

	printf("`pool_seq_test` deinit\n");
}
