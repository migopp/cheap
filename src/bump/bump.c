#include "bump.h"
#include <stdint.h>
#include <stddef.h>

uint8_t cheap_bump[CHEAP_BUMP_SIZE];
size_t cheap_bump_idx = 0;
size_t cheap_bump_mallocc = 0;

void *bump_malloc(size_t size) {
	cheap_bump_mallocc++;
	void *ptr = (void *)&cheap_bump[cheap_bump_idx];
	cheap_bump_idx += size;
	return ptr;
}

size_t get_bump_mallocc() { return cheap_bump_mallocc; }
