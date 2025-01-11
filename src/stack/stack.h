#ifndef _CHEAP_STACK
#define _CHEAP_STACK
#include <stddef.h>

// Settings
#define CHEAP_STACK_SIZE (1 << 20)
#define CHEAP_STACK_FRAME_SIZE 8

typedef struct stack_allocator stack_allocator;

stack_allocator *stack_init(void);
void stack_deinit(stack_allocator *);

void *stack_malloc(stack_allocator *, size_t);
void stack_free(stack_allocator *, void *);

size_t stack_malloc_count(stack_allocator *);
size_t stack_free_count(stack_allocator *);
#endif	// !_CHEAP_STACK
