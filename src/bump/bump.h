#ifndef _CHEAP_BUMP
#define _CHEAP_BUMP
#include <stddef.h>

#define CHEAP_BUMP_SIZE (1 << 10)

void *bump_malloc(size_t);
size_t get_bump_mallocc();
#endif	// !_CHEAP_BUMP
