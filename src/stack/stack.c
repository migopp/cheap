#include "stack.h"
#include "allocator.h"
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/mman.h>

struct stack_allocator {
	AllocatorType a_type;
	uint8_t *stack_heap;
	uint8_t *stack_sp;
	size_t stack_mallocc;
	size_t stack_freec;
};

bool stack_in_bounds_left(stack_allocator *a, void *p) {
	return p >= (void *)a->stack_heap;
}

bool stack_in_bounds_right(stack_allocator *a, void *p) {
	return p < (void *)&a->stack_heap[CHEAP_STACK_SIZE];
}

bool stack_in_bounds(stack_allocator *a, void *p) {
	return stack_in_bounds_left(a, p) && stack_in_bounds_right(a, p);
}

stack_allocator *stack_init(void) {
	// Make space for the actual allocator object
	//
	// Would use `sbrk` but its depricated? `mmap` seems like a
	// tad bit of a waste, since it'll give us a whole page,
	// but oh well.
	stack_allocator *a =
		mmap(NULL, sizeof(stack_allocator), PROT_READ | PROT_WRITE,
			 MAP_ANON | MAP_PRIVATE, -1, 0);
	if (a == MAP_FAILED) {
		return NULL;
	}

	// Create heap
	//
	// Now, this is more of a job for `mmap`.
	uint8_t *h = mmap(NULL, CHEAP_STACK_SIZE, PROT_READ | PROT_WRITE,
					  MAP_ANON | MAP_PRIVATE, -1, 0);
	if (h == MAP_FAILED) {
		munmap(a, sizeof(stack_allocator));
		return NULL;
	}

	// Init heap data
	a->a_type = STACK;
	a->stack_heap = h;
	a->stack_sp = h;
	a->stack_mallocc = 0;
	a->stack_freec = 0;

	return a;
}

void stack_deinit(stack_allocator *a) {
	// Unmap our segments
	munmap(a->stack_heap, CHEAP_STACK_SIZE);
	munmap(a, sizeof(stack_allocator));
}

void *stack_malloc(stack_allocator *a, size_t size) {
	// Check for space
	if (!stack_in_bounds_right(a, a->stack_sp + size)) return NULL;

	// Increment and return
	void *ptr = a->stack_sp;
	a->stack_sp += size;
	a->stack_mallocc++;
	return ptr;
}

void stack_free(stack_allocator *a, void *ptr) {
	// Check bounds
	if (!stack_in_bounds(a, ptr)) return;

	// Regress stack pointer
	a->stack_sp = ptr;
	a->stack_freec++;
}

size_t stack_malloc_count(stack_allocator *a) { return a->stack_mallocc; }

size_t stack_free_count(stack_allocator *a) { return a->stack_freec; }
