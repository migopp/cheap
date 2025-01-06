#ifndef _CHEAP_ASSERT
#define _CHEAP_ASSERT
#include <stdlib.h>
#include <stdio.h>
#define ASSERT(condition)                                                 \
	if (!(condition)) {                                                   \
		fprintf(stderr,                                                   \
				"ASSERTION FAILED (%s), function %s, file %s, line %d\n", \
				#condition, __func__, __FILE__, __LINE__);                \
		exit(0);                                                          \
	}
#endif	// !_CHEAP_ASSERT
