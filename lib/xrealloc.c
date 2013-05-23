#include "config.h"

#include <assert.h>
#include <err.h>
#include <stdlib.h>

/* This function is fully replaced by libc.
 * If you are using readline, please, don`t use it.
 * Resize an allocated memory area.
 */

__inline__ void *xrealloc(void *ptr, size_t size)
{
	void *newptr;
	
	assert(ptr != NULL);
	assert(size > 0);
	
	if ((newptr = realloc(ptr, size)) == NULL)
		err(1, NULL);
	
	return newptr;
}

