#ifndef _CHEAP_STACK
#define _CHEAP_STACK
#include <stddef.h>

// Heap size, in bytes
#define CHEAP_STACK_SIZE (1 << 20)

void *stack_malloc(size_t);
void stack_free(void *);
size_t get_stack_mallocc();
size_t get_stack_freec();
#endif	// !_CHEAP_STACK
