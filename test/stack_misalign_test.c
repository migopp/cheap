#include "stack/stack.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_STACK
#error "STACK ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`stack_misalign_test` init\n");
	stack_allocator a = stack_init();

	// Allocate something that we may be temped to not align
	uint8_t *f = (uint8_t *)stack_malloc(&a, 1);
	ASSERT(f != NULL);
	int *n = (int *)stack_malloc(&a, sizeof(int));
	ASSERT(n != NULL);

	// Try to write something into it
	*f = 0;
	*n = 1;

	stack_deinit(&a);
	printf("`stack_misalign_test` deinit\n");
}
