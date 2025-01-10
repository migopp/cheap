#include "arena/arena.h"
#include <stdint.h>
#include <time.h>
#include <stdio.h>

#define BENCH_ITERS 100
#define MALLOC_COUNT (1 << 10)

AllocatorType types[] = {CHEAP_BUDDY, CHEAP_BUMP, CHEAP_FL, CHEAP_POOL,
						 CHEAP_STACK};

typedef int (*bench_routine)(arena_allocator *a);

int m(arena_allocator *a) {
	uint8_t *c[MALLOC_COUNT];
	for (size_t i = 0; i < MALLOC_COUNT; ++i) {
		c[i] = arena_malloc(a, 1);
		if (c[i] == NULL) return -1;
	}
	return 0;
}

int mf(arena_allocator *a) {
	uint8_t *c[MALLOC_COUNT];
	for (size_t i = 0; i < MALLOC_COUNT; ++i) {
		c[i] = arena_malloc(a, 1);
		if (c[i] == NULL) return -1;
	}
	for (size_t i = 0; i < MALLOC_COUNT; ++i) arena_free(a, c[i]);
	return 0;
}

void bench_type(AllocatorType ty, bench_routine r) {
	// Init allocator
	arena_allocator *a = arena_init(ty);

	// Time routine
	clock_t b_t = clock();
	int rc = r(a);
	clock_t e_t = clock();
	double t = ((double)(e_t - b_t)) / CLOCKS_PER_SEC;

	// Print result
	printf("%d: %f %d\n", ty, t, rc);

	// Deinit allocator
	arena_deinit(a);
}

void bench_with(bench_routine r) {
	// Bench each type
	for (size_t at = 0; at < 5; ++at) {
		// With this many iterations
		for (size_t bi = 0; bi < BENCH_ITERS; ++bi) {
			// Running this function
			bench_type(types[at], r);
		}
	}
}

int main() {
	bench_with(m);
	bench_with(mf);
	return 0;
}
