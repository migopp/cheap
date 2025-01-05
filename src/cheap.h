#ifndef cheap
#define cheap
#include <stdint.h>
#include <stddef.h>

// Size of the heap in bytes
#define CHEAP_SIZE 1000

// The actual heap itself...
// It's _just_ an array of bytes!
extern uint8_t _cheap[];

// State trackers
extern size_t _malloc_count;
extern size_t _free_count;

void _cheap_init();
void *malloc(size_t);
void free(void *);
#endif	// !cheap
