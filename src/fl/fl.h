#ifndef _CHEAP_FL
#define _CHEAP_FL
#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

// Settings
#define CHEAP_FL_SIZE (1 << 20)
#define CHEAP_FL_FRAME_SIZE 8
#define CHEAP_FL_SPLIT_MIN (1 << 5)

// Each free list block has the following layout in memory:
//
//   8B   8B                 nB                8B
// ________________________________________________
// | sz | nx |              data             | he |
// ------------------------------------------------
//
// Where `sz`, `nx`, and `he` are metadata, and:
// 		- `sz` is the size of `data`,
// 		- `nx` is a pointer to the _next_ free block,
// 		- `he` is a pointer to the head of the block.
//
// `sz` is negative to indicate that a block is allocated.
typedef struct fl_head_md {
	ssize_t flb_size;			  // sz
	struct fl_head_md *flb_next;  // nx
} fl_head_md;
typedef struct fl_tail_md {
	fl_head_md *flb_head;  // he
} fl_tail_md;

typedef struct fl_allocator {
	uint8_t *fl_heap;
	fl_head_md *fl_first;
	size_t fl_mallocc;
	size_t fl_freec;
	uint8_t fl_valid;
} fl_allocator;

fl_allocator fl_init(void);
void fl_deinit(fl_allocator *a);

void *fl_malloc(fl_allocator *, size_t);
void fl_free(fl_allocator *, void *);

size_t fl_malloc_count(fl_allocator *);
size_t fl_free_count(fl_allocator *);
#endif	// !_CHEAP_FL
