#ifndef ALLOC_DEBUG_H__
#define ALLOC_DEBUG_H__

#include <stdio.h>
#include <stdlib.h>


/*
 * Simplistic macros to help finding `hot spots'/bugs in memory handling.
 * Require that MALLOC(x) is never a single statement following an if(),
 * else() etc. without braces, and never in the middle of a variable
 * declarations list.
 */

/* #define DEBUG_MEMORY_HANDLING */

#ifdef DEBUG_MEMORY_HANDLING
#define XMALLOC(x,y) ( \
	fprintf(stderr, "=== %s:%d: malloc(%p (%s), %d)\n", __FILE__, __LINE__, x, #x, y), \
	xmalloc(y))
#define XREALLOC(x,y) ( \
	fprintf(stderr, "=== %s:%d: realloc(%p (%s), %d)\n", __FILE__, __LINE__, x, #x, y), \
	xrealloc(x, y))
#define FREE(x)	( \
	fprintf(stderr, "=== %s:%d: free(%p (%s))\n", __FILE__, __LINE__, x, #x), \
	free(x))
#else
#define XMALLOC(x,y) xmalloc(y)
#define XREALLOC(x,y) xrealloc(x,y)
#define FREE(x) free(x)
#endif

#endif /* ALLOC_DEBUG_H__ */
