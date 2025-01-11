#include "buddy/buddy.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_BUDDY
#error "BUDDY ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`buddy_coal_test` init\n");
	buddy_allocator a = buddy_init();

	// Fill up heap space
	const size_t CHEAP_BUDDY_LEAVES = 1 << CHEAP_BUDDY_ORDERS;
	uint8_t *c[CHEAP_BUDDY_LEAVES];
	for (size_t i = 0; i < CHEAP_BUDDY_LEAVES; ++i) {
		c[i] = (uint8_t *)buddy_malloc(&a, sizeof(uint8_t));
		ASSERT(c[i] != NULL);
	}

	// Try one more allocation
	// (it should fail...)
	uint8_t *fail = (uint8_t *)buddy_malloc(&a, sizeof(uint8_t));
	ASSERT(fail == NULL);

	// Free everything
	for (size_t i = 0; i < CHEAP_BUDDY_LEAVES; ++i) {
		buddy_free(&a, c[i]);
	}

	// Try a large allocation
	const size_t CHEAP_BUDDY_TOTAL_SIZE =
		CHEAP_BUDDY_BLOCK_SIZE * CHEAP_BUDDY_LEAVES;
	uint8_t *big = (uint8_t *)buddy_malloc(&a, CHEAP_BUDDY_TOTAL_SIZE);
	ASSERT(big != NULL);

	// Match counts
	size_t buddy_mallocc;
	if ((buddy_mallocc = buddy_malloc_count(&a)) != CHEAP_BUDDY_LEAVES + 1) {
		fprintf(stderr, "wrong `buddy_mallocc`: %zu\n", buddy_mallocc);
	}
	size_t buddy_freec;
	if ((buddy_freec = buddy_free_count(&a)) != CHEAP_BUDDY_LEAVES) {
		fprintf(stderr, "wrong `buddy_freec`: %zu\n", buddy_freec);
	}

	buddy_deinit(&a);
	printf("`buddy_coal_test` deinit\n");
}
