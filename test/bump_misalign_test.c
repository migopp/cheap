#include "bump/bump.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_BUMP
#error "BUMP ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`bump_misalign_test` init\n");
	bump_allocator *a = bump_init();

	// Allocate something that we may be temped to not align
	uint8_t *f = (uint8_t *)bump_malloc(a, 1);
	ASSERT(f != NULL);
	int *n = (int *)bump_malloc(a, sizeof(int));
	ASSERT(n != NULL);

	// Try to write something into it
	*f = 0;
	*n = 1;

	bump_deinit(a);
	printf("`bump_misalign_test` deinit\n");
}
