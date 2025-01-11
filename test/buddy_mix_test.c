#include "buddy/buddy.h"
#include "assert.h"
#include <stdint.h>
#include <stdio.h>

#ifndef _CHEAP_BUDDY
#error "BUDDY ALLOCATOR NOT DEFINED"
#endif

int main() {
	printf("`buddy_mix_test` init\n");
	buddy_allocator a = buddy_init();

	// Fill up heap space
	const size_t CHEAP_BUDDY_LEAVES_H = 1 << (CHEAP_BUDDY_ORDERS - 1);
	const size_t CHEAP_BUDDY_SIZE_H =
		CHEAP_BUDDY_BLOCK_SIZE * CHEAP_BUDDY_LEAVES_H;
	uint8_t *f = (uint8_t *)buddy_malloc(&a, CHEAP_BUDDY_SIZE_H);
	ASSERT(f != NULL);
	uint8_t *n = (uint8_t *)buddy_malloc(&a, CHEAP_BUDDY_SIZE_H);
	ASSERT(n != NULL);

	// Try one more allocation
	// (it should fail...)
	uint8_t *fail = (uint8_t *)buddy_malloc(&a, sizeof(uint8_t));
	ASSERT(fail == NULL);

	// Fill `f` and `n`
	for (size_t i = 0; i < CHEAP_BUDDY_SIZE_H; ++i) f[i] = 1;
	for (size_t i = 0; i < CHEAP_BUDDY_SIZE_H; ++i) n[i] = 2;

	// Check that `f` was changed
	for (size_t i = 0; i < CHEAP_BUDDY_SIZE_H; ++i) ASSERT(f[i] == 1);

	// Match counts
	size_t buddy_mallocc;
	if ((buddy_mallocc = buddy_malloc_count(&a)) != 2) {
		fprintf(stderr, "wrong `buddy_mallocc`: %zu\n", buddy_mallocc);
	}
	size_t buddy_freec;
	if ((buddy_freec = buddy_free_count(&a)) != 0) {
		fprintf(stderr, "wrong `buddy_freec`: %zu\n", buddy_freec);
	}

	buddy_deinit(&a);
	printf("`buddy_mix_test` deinit\n");
}
