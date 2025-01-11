#include "bump/bump.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_BUMP
#error "BUMP ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`bump_wraparound_test` init\n");
	bump_allocator a = bump_init();

	// Make an allocation that is guaranteed to overflow the address space
	size_t size = 0xffffffffffffffff;
	uint8_t *f = (uint8_t *)bump_malloc(&a, size);
	ASSERT(f == NULL);

	bump_deinit(&a);
	printf("`bump_wraparound_test` deinit\n");
}
