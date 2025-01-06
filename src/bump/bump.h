#ifndef _CHEAP_BUMP
#define _CHEAP_BUMP
#include <stddef.h>

#ifndef CHEAP_BUMP_SIZE
#define CHEAP_BUMP_SIZE 1 << 10
#endif	// !CHEAP_BUMP_SIZE

void *bump_malloc(size_t);
size_t get_bump_mallocc();
#endif	// !_CHEAP_BUMP
