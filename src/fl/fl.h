#ifndef _CHEAP_FL
#define _CHEAP_FL
#include <stddef.h>

// Settings
#define CHEAP_FL_SIZE (1 << 20)
#define CHEAP_FL_FRAME_SIZE 8
#define CHEAP_FL_SPLIT_MIN (1 << 5)

typedef struct fl_allocator fl_allocator;

fl_allocator *fl_init(void);
void fl_deinit(fl_allocator *a);

void *fl_malloc(fl_allocator *, size_t);
void fl_free(fl_allocator *, void *);

size_t fl_malloc_count(fl_allocator *);
size_t fl_free_count(fl_allocator *);
#endif	// !_CHEAP_FL
