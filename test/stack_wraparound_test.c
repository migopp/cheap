#include "stack/stack.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_STACK
#error "STACK ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`stack_wraparound_test` init\n");
	stack_allocator a = stack_init();

	// Make an allocation that is guaranteed to overflow the address space
	size_t size = 0xffffffffffffffff;
	uint8_t *f = (uint8_t *)stack_malloc(&a, size);
	ASSERT(f == NULL);

	stack_deinit(&a);
	printf("`stack_wraparound_test` deinit\n");
}
