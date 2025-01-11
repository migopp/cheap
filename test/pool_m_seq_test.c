#include "pool/pool.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_POOL
#error "POOL ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`pool_m_seq_test` init\n");
	pool_allocator a = pool_init();

	// Allocate
	uint8_t *f = (uint8_t *)pool_malloc(&a, 33 * sizeof(uint8_t));
	ASSERT(f != NULL);
	uint8_t *n = (uint8_t *)pool_malloc(&a, 511 * sizeof(uint8_t));
	ASSERT(n != NULL);

	// Check sizes
	//
	// This is technically undefined, but I know that
	// this is metadata because I implemented it.
	size_t f_size = *(size_t *)(f - 16);
	size_t n_size = *(size_t *)(n - 16);
	ASSERT(f_size == CHEAP_POOL_M);
	ASSERT(n_size == CHEAP_POOL_M);

	// Free our blocks
	pool_free(&a, f);
	pool_free(&a, n);

	// Match counts
	size_t pool_mallocc;
	if ((pool_mallocc = pool_malloc_count(&a)) != 2) {
		fprintf(stderr, "wrong `pool_mallocc`: %zu\n", pool_mallocc);
	}
	size_t pool_freec;
	if ((pool_freec = pool_free_count(&a)) != 2) {
		fprintf(stderr, "wrong `pool_freec`: %zu\n", pool_freec);
	}

	pool_deinit(&a);
	printf("`pool_m_seq_test` deinit\n");
}
