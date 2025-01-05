#include "bump.h"
#include <stddef.h>
#include <stdio.h>
#include "cheap.h"

static size_t _bump_idx = 0;

void _cheap_init() {}

void *malloc(size_t size) {
	_malloc_count++;
	void *ptr = (void *)&_cheap[_bump_idx];
	_bump_idx += size;
	return ptr;
}

void free(void *ptr) {
	_free_count++;
	(void)ptr;
}
