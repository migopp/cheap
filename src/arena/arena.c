#include "arena.h"
#include "buddy/buddy.h"
#include "bump/bump.h"
#include "fl/fl.h"
#include "pool/pool.h"
#include "stack/stack.h"
#include <sys/mman.h>

struct arena_allocator {
	AllocatorType a_type;
	void *a_alloc;
};

arena_allocator *arena_init(AllocatorType type) {
	// Make space for the actual allocator object
	//
	// Would use `sbrk` but its depricated on OSX? `mmap`
	// seems like a tad bit of a waste, since it'll give
	// us a whole page, but oh well.
	arena_allocator *a =
		mmap(NULL, sizeof(arena_allocator), PROT_READ | PROT_WRITE,
			 MAP_ANON | MAP_PRIVATE, -1, 0);
	if (a == MAP_FAILED) {
		return NULL;
	}

	// Create `a_alloc`, the underlying allocator
	void *na = NULL;
	switch (type) {
		case CHEAP_BUDDY: {
			na = (void *)buddy_init();
			break;
		}
		case CHEAP_BUMP: {
			na = (void *)bump_init();
			break;
		}
		case CHEAP_FL: {
			na = (void *)fl_init();
			break;
		}
		case CHEAP_POOL: {
			na = (void *)pool_init();
			break;
		}
		case CHEAP_STACK: {
			na = (void *)stack_init();
			break;
		}
		default:
			return NULL;
	}

	// `init` failed
	if (na == NULL) return NULL;

	// Save the underlying allocator
	a->a_type = type;
	a->a_alloc = na;
	return a;
}

void arena_deinit(arena_allocator *a) {
	if (!a) return;

	// Call `deinit` manually
	switch (a->a_type) {
		case CHEAP_BUDDY:
			buddy_deinit(a->a_alloc);
			break;
		case CHEAP_BUMP:
			bump_deinit(a->a_alloc);
			break;
		case CHEAP_FL:
			fl_deinit(a->a_alloc);
			break;
		case CHEAP_POOL:
			pool_deinit(a->a_alloc);
			break;
		case CHEAP_STACK:
			stack_deinit(a->a_alloc);
			break;
		default:
			break;
	}
}

void *arena_malloc(arena_allocator *a, size_t size) {
	if (!a) return NULL;

	// Call `malloc` manually
	switch (a->a_type) {
		case CHEAP_BUDDY:
			return buddy_malloc(a->a_alloc, size);
		case CHEAP_BUMP:
			return bump_malloc(a->a_alloc, size);
		case CHEAP_FL:
			return fl_malloc(a->a_alloc, size);
		case CHEAP_POOL:
			return pool_malloc(a->a_alloc, size);
		case CHEAP_STACK:
			return stack_malloc(a->a_alloc, size);
		default:
			return NULL;
	}
}

void arena_free(arena_allocator *a, void *ptr) {
	if (!a) return;

	// Call `free` manually
	switch (a->a_type) {
		case CHEAP_BUDDY:
			buddy_free(a->a_alloc, ptr);
			return;
		case CHEAP_FL:
			fl_free(a->a_alloc, ptr);
			return;
		case CHEAP_POOL:
			pool_free(a->a_alloc, ptr);
			return;
		case CHEAP_STACK:
			stack_free(a->a_alloc, ptr);
			return;
		default:
			return;
	}
}

size_t arena_malloc_count(arena_allocator *a) {
	if (!a) return 0;

	// You know the drill...
	switch (a->a_type) {
		case CHEAP_BUDDY:
			return buddy_malloc_count(a->a_alloc);
		case CHEAP_BUMP:
			return bump_malloc_count(a->a_alloc);
		case CHEAP_FL:
			return fl_malloc_count(a->a_alloc);
		case CHEAP_POOL:
			return pool_malloc_count(a->a_alloc);
		case CHEAP_STACK:
			return stack_malloc_count(a->a_alloc);
		default:
			return 0;
	}
}

size_t arena_free_count(arena_allocator *a) {
	if (!a) return 0;

	// Again...
	switch (a->a_type) {
		case CHEAP_BUDDY:
			return buddy_free_count(a->a_alloc);
		case CHEAP_FL:
			return fl_free_count(a->a_alloc);
		case CHEAP_POOL:
			return pool_free_count(a->a_alloc);
		case CHEAP_STACK:
			return stack_free_count(a->a_alloc);
		default:
			return 0;
	}
}
