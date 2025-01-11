#include "stack/stack.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_STACK
#error "STACK ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`stack_space_exceeded_test` init\n");
	stack_allocator a = stack_init();

	// Fail to allocate!
	//
	// We should not have enough room.
	uint8_t *p = (uint8_t *)stack_malloc(&a, (1 << 21) * sizeof(uint8_t));
	if (p != NULL) {
		fprintf(stderr, "allocation succeeded when it was meant to fail: %p\n",
				p);
	}

	// Match counts
	size_t bump_mallocc;
	if ((bump_mallocc = stack_malloc_count(&a)) != 0) {
		fprintf(stderr, "wrong `stack_mallocc`: %zu\n", bump_mallocc);
	}

	stack_deinit(&a);
	printf("`stack_space_exceeded_test` deinit\n");
}
