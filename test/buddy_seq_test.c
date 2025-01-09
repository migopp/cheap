#include "buddy/buddy.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_BUDDY
#error "BUDDY ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`buddy_seq_test` init\n");
	buddy_allocator *a = buddy_init();

	// Allocate
	uint8_t *f = (uint8_t *)buddy_malloc(a, sizeof(uint8_t));
	ASSERT(f != NULL);
	uint8_t *m = (uint8_t *)buddy_malloc(a, sizeof(uint8_t));
	ASSERT(m != NULL);

	// Match counts
	size_t buddy_mallocc;
	if ((buddy_mallocc = buddy_malloc_count(a)) != 2) {
		fprintf(stderr, "wrong `buddy_mallocc`: %zu\n", buddy_mallocc);
	}
	size_t buddy_freec;
	if ((buddy_freec = buddy_free_count(a)) != 0) {
		fprintf(stderr, "wrong `buddy_freec`: %zu\n", buddy_freec);
	}

	buddy_deinit(a);
	printf("`buddy_seq_test` deinit\n");
}
