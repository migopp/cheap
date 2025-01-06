#include "fl.h"
#include <stdint.h>
#include <stddef.h>

uint8_t cheap_fl[CHEAP_FL_SIZE];

void *fl_malloc(size_t size) {
	(void)size;
	return NULL;
}

void fl_free(void *ptr) { (void)ptr; }

size_t get_fl_mallocc() { return 0; }

size_t get_fl_freec() { return 0; }
