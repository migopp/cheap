#include "stack.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

uint8_t cheap_stack[CHEAP_STACK_SIZE];
void *cheap_sp = cheap_stack;
size_t cheap_stack_mallocc = 0;
size_t cheap_stack_freec = 0;

bool stack_in_bounds_left(void *p) { return p >= (void *)cheap_stack; }

bool stack_in_bounds_right(void *p) {
	return p < (void *)&cheap_stack[CHEAP_STACK_SIZE];
}

bool stack_in_bounds(void *p) {
	return stack_in_bounds_left(p) && stack_in_bounds_right(p);
}

void *stack_malloc(size_t size) {
	// Check for space
	if (!stack_in_bounds_right(cheap_sp + size)) return NULL;

	// Increment and return
	void *ptr = cheap_sp;
	cheap_sp += size;
	cheap_stack_mallocc++;
	return ptr;
}

void stack_free(void *ptr) {
	// Check bounds
	if (!stack_in_bounds(ptr)) return;

	// Regress stack pointer
	cheap_sp = ptr;
	cheap_stack_freec++;
}

size_t get_stack_mallocc() { return cheap_stack_mallocc; }

size_t get_stack_freec() { return cheap_stack_freec; }
