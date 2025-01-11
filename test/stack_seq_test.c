#include "stack/stack.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_STACK
#error "STACK ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`stack_seq_test` init\n");
	stack_allocator a = stack_init();

	// Allocate
	uint8_t *f = (uint8_t *)stack_malloc(&a, sizeof(uint8_t));
	ASSERT(f != NULL);
	uint8_t *n = (uint8_t *)stack_malloc(&a, sizeof(uint8_t));
	ASSERT(n != NULL);

	// Free the first allocation
	//
	// If we free `f`, it should also free `n`.
	stack_free(&a, f);
	uint8_t *m = (uint8_t *)stack_malloc(&a, sizeof(uint8_t));
	ASSERT(f == m);

	// Match counts
	size_t stack_mallocc;
	if ((stack_mallocc = stack_malloc_count(&a)) != 3) {
		fprintf(stderr, "wrong `stack_mallocc`: %zu\n", stack_mallocc);
	}
	size_t stack_freec;
	if ((stack_freec = stack_free_count(&a)) != 1) {
		fprintf(stderr, "wrong `stack_freec`: %zu\n", stack_freec);
	}

	stack_deinit(&a);
	printf("`stack_seq_test` deinit\n");
}
