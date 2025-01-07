#include "stack/stack.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_STACK
#error "STACK ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`stack_seq_test` init\n");

	// Allocate
	uint8_t *f = (uint8_t *)stack_malloc(sizeof(uint8_t));
	ASSERT(f != NULL);
	uint8_t *n = (uint8_t *)stack_malloc(sizeof(uint8_t));
	ASSERT(n != NULL);
	// If we bump, consecutive allocations should be
	// neighboring
	//
	// So there should be 1 byte between `f` and `n`
	if (f != n - 1) {
		fprintf(stderr, "%p != %p - 1\n", f, n);
	}

	// Free the first allocation
	//
	// If we free `f`, it should also free `n`.
	stack_free(f);
	uint8_t *m = (uint8_t *)stack_malloc(sizeof(uint8_t));
	ASSERT(f == m);

	// Match counts
	size_t stack_mallocc;
	if ((stack_mallocc = get_stack_mallocc()) != 3) {
		fprintf(stderr, "wrong `stack_mallocc`: %zu\n", stack_mallocc);
	}
	size_t stack_freec;
	if ((stack_freec = get_stack_freec()) != 1) {
		fprintf(stderr, "wrong `stack_freec`: %zu\n", stack_freec);
	}

	printf("`stack_seq_test` deinit\n");
}
