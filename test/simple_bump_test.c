#include "bump.h"
#include "cheap.h"
#include <stdint.h>
#include <stdio.h>

#ifndef cheap_bump
#error "BUMP ALLOCATOR NOT DEFINED"
#endif

int main(void) {
	printf("*** `simple_bump_test` init\n");
	uint8_t *f = malloc(sizeof(uint8_t));
	uint8_t *n = malloc(sizeof(uint8_t));
	if (f != n - 1) {
		printf("xxx %p != %p - 1\n", f, n);
	}

	free(f);
	free(n);

	if (_malloc_count != 3) {
		printf("xxx wrong malloc count: %zu\n", _malloc_count);
	}
	if (_free_count != 2) {
		printf("xxx wrong free count: %zu\n", _free_count);
	}

	printf("*** `simple_bump_test` deinit\n");
}
