#ifndef _CHEAP_FL
#define _CHEAP_FL
#include <stddef.h>

#ifndef CHEAP_FL_SIZE
#define CHEAP_FL_SIZE 1 << 20
#endif	// !CHEAP_FL_SIZE

void fl_init();
void *fl_malloc(size_t);
void fl_free(void *);
size_t get_fl_mallocc();
size_t get_fl_freec();
#endif	// !_CHEAP_FL
