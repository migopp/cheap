#ifndef _CHEAP_STACK
#define _CHEAP_STACK
#include <stddef.h>

// Settings
#define CHEAP_STACK_SIZE (1 << 20)

// Allocator object
typedef struct stack_allocator stack_allocator;

// CD
stack_allocator *stack_init(void);
void stack_deinit(stack_allocator *);

// MMAN
void *stack_malloc(stack_allocator *, size_t);
void stack_free(stack_allocator *, void *);

// Debug
size_t stack_malloc_count(stack_allocator *);
size_t stack_free_count(stack_allocator *);
#endif	// !_CHEAP_STACK
