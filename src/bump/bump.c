#include "bump.h"
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>

uint8_t cheap_bump[CHEAP_BUMP_SIZE];
size_t cheap_bump_idx = 0;
size_t bump_mallocc = 0;

void *bump_malloc(size_t size) {
	// Alas, for the bump allocator this comes too soon!
	if (cheap_bump_idx + size > CHEAP_BUMP_SIZE) return NULL;

	// Bump it.
	void *ptr = (void *)&cheap_bump[cheap_bump_idx];
	cheap_bump_idx += size;
	bump_mallocc++;
	return ptr;
}

size_t get_bump_mallocc() { return bump_mallocc; }
