#ifndef _CHEAP_FL
#define _CHEAP_FL
#include <stddef.h>

// Heap size, in bytes
#define CHEAP_FL_SIZE (1 << 20)

// Block frame size
#define CHEAP_FL_FRAME_SIZE 8

// The smallest acceptable block size
#define CHEAP_FL_SPLIT_MIN (1 << 4)

void fl_init();
void *fl_malloc(size_t);
void fl_free(void *);
size_t get_fl_mallocc();
size_t get_fl_freec();
#endif	// !_CHEAP_FL
