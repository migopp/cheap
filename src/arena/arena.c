#include "arena.h"
#include "buddy/buddy.h"
#include "bump/bump.h"
#include "fl/fl.h"
#include "pool/pool.h"
#include "stack/stack.h"
#include <sys/mman.h>

arena_allocator arena_init(AllocatorType type) {
	arena_allocator a;
	a.a_type = NONE;

	// Create `a_alloc`, the underlying allocator
	void *na = NULL;
	switch (type) {
		case CHEAP_BUDDY: {
			buddy_allocator ia = buddy_init();
			buddy_allocator *aa = buddy_malloc(&ia, sizeof(buddy_allocator));
			*aa = ia;
			na = (void *)aa;
			break;
		}
		case CHEAP_BUMP: {
			bump_allocator ia = bump_init();
			bump_allocator *aa = bump_malloc(&ia, sizeof(bump_allocator));
			*aa = ia;
			na = (void *)aa;
			break;
		}
		case CHEAP_FL: {
			fl_allocator ia = fl_init();
			fl_allocator *aa = fl_malloc(&ia, sizeof(fl_allocator));
			*aa = ia;
			na = (void *)aa;
			break;
		}
		case CHEAP_POOL: {
			pool_allocator ia = pool_init();
			pool_allocator *aa = pool_malloc(&ia, sizeof(pool_allocator));
			*aa = ia;
			na = (void *)aa;
			break;
		}
		case CHEAP_STACK: {
			stack_allocator ia = stack_init();
			stack_allocator *aa = stack_malloc(&ia, sizeof(stack_allocator));
			*aa = ia;
			na = (void *)aa;
			break;
		}
		default:
			return a;
	}

	// Save the underlying allocator
	a.a_type = type;
	a.a_alloc = na;
	return a;
}

void arena_deinit(arena_allocator *a) {
	if (!a || a->a_type == NONE) return;

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
	if (!a || a->a_type == NONE) return NULL;

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
	if (!a || a->a_type == NONE) return;

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
	if (!a || a->a_type == NONE) return 0;

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
	if (!a || a->a_type == NONE) return 0;

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
