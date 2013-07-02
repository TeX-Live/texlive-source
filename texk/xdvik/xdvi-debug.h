#ifndef XDVI_DEBUG_H_
#define XDVI_DEBUG_H_

/*
 * Copyright (c) 2002-2013 the xdvik development team
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * PAUL VOJTA OR ANY OTHER AUTHOR OF THIS SOFTWARE BE LIABLE FOR ANY CLAIM,
 * DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/* debugging flags and macros */

#include "xdvi.h"
#include "version.h"

#if HAVE_STRINGIZE
#ifdef NDEBUG
/* for production code, a failed ASSERT(x) just prints out a warning;
   else it aborts and dumps core.
*/
#define ASSERT(x, y) do {								\
    if(!(x)) {										\
	fprintf(stderr,									\
		"************************************************************\n"	\
		"XDvi %s: Failed assertion:\n%s:%d: \"%s\": %s\n"			\
		"Please report this as a bug to:\n"					\
		"http://sourceforge.net/tracker/?group_id=23164&atid=377580\n"		\
 		"************************************************************\n",	\
		XDVI_VERSION_INFO, __FILE__, __LINE__, #x, y);				\
    } } while (0)
#else /* NDEBUG */
#define ASSERT(x, y) do {								\
    if(!(x)) {										\
	fprintf(stderr,									\
		"\n************************************************************\n"	\
		"XDvi %s: Failed assertion:\n%s:%d: \"%s\": %s\n"			\
		"Aborting now. Please report this as a bug to:\n"			\
		"http://sourceforge.net/tracker/?group_id=23164&atid=377580\n"		\
		"If a core dump has been produced, please invoke:\n"			\
		"gdb %s core\nThen type \"bt\", "					\
		"and include the resulting output in your bug report.\n"		\
 		"************************************************************\n",	\
		XDVI_VERSION_INFO, __FILE__, __LINE__, #x, y, globals.program_name);	\
	do_abort();									\
    } } while (0)
#endif /* NDEBUG */
#else /* HAVE_STRINGIZE */
#define ASSERT(x, y) /* as nothing */
#endif

/* for temporary debugging statements */
/* #define MYDEBUG 1 */
#ifdef MYDEBUG
# define MYTRACE(X)   do {                          \
    fprintf(stderr, "%s:%d: ", __FILE__, __LINE__); \
    fprintf X;                                      \
    fprintf(stderr, "\n");                          \
} while(0)
#else
# define MYTRACE(X)
#endif

/* NOTE: keep these in sync with the debug_string_options array in util.c! */
#define	DBG_BITMAP		1
#define	DBG_DVI			2
#define	DBG_PK			4
#define	DBG_BATCH		8
#define	DBG_EVENT		16
#define	DBG_PS			32
/* start of kpathsea debugging options */
#define	DBG_STAT		64
#define	DBG_HASH		128
#define	DBG_OPEN		256
#define	DBG_PATHS		512
#define	DBG_EXPAND		1024
#define	DBG_SEARCH		2048
#define	DBG_KPATHSEA		4032	/* handy abbrev */
/* end of kpathsea debugging options */
#define	DBG_HTEX		4096
#define DBG_SRC_SPECIALS	8192
#define DBG_CLIENT		16384
#define DBG_FT			32768
#define DBG_FT_VERBOSE		65536	/* not currently used */
#define DBG_GUI			131072
#define DBG_FIND		262144
#define DBG_FILES		524288
#define	DBG_ALL			(~DBG_BATCH)

/* a mapping of numerical options to descriptive strings, defined in util.c */
struct debug_string_options {
    int bitmap;
    const char *description;
    const char *help_formatting;
};

/*
 */

#if 0
/* we don't want this defined for NDEBUG, since the tracing macros
   are pretty useful also for users reporting bugs etc.; so we 
   enable them always.
*/
#define TRACE_HTEX(X)
#define TRACE_SRC(X)
#define TRACE_CLIENT(X)
#define TRACE_FT(X)
#define TRACE_FT_VERBOSE(X)
#define TRACE_GUI(X)
#define TRACE_EVENTS(X)
#define TRACE_FIND(X)
#define TRACE_FILES(X)

#else /* 0 */

/*
 * Note that the argument to these macros is always ((stderr, "...")).
 * We could also have
 *    TRACE_SRC(("..."));
 * and invoke a function, but then gcc (3.1) won't be able to check
 * inconsistencies between conversion specifiers and arguments any
 * more, and that's a real killer IMHO.
 */

#define TRACE_HTEX(X)						    \
    do {							    \
	if (globals.debug & DBG_HTEX) {					    \
	    fprintf(stderr, "%s:%d: HTEX: ", __FILE__, __LINE__);   \
	    fprintf X;						    \
	    fprintf(stderr, "\n");				    \
	}							    \
    } while(0)
#define TRACE_SRC(X)						    \
    do {							    \
	if (globals.debug & DBG_SRC_SPECIALS) {				    \
	    fprintf(stderr, "%s:%d: SRC: ", __FILE__, __LINE__);    \
	    fprintf X;						    \
	    fprintf(stderr, "\n");				    \
	}							    \
    } while(0)
#define TRACE_CLIENT(X)						    \
    do {							    \
	if (globals.debug & DBG_CLIENT) {				    \
	    fprintf(stderr, "%s:%d: CLIENT: ", __FILE__, __LINE__); \
	    fprintf X;						    \
	    fprintf(stderr, "\n");				    \
	}							    \
    } while(0)
#define TRACE_FT(X)						    \
    do {							    \
	if (globals.debug & DBG_FT) {					    \
	    fprintf(stderr, "%s:%d: FT: ", __FILE__, __LINE__);	    \
	    fprintf X;						    \
	    fprintf(stderr, "\n");				    \
	}							    \
    } while(0)
#define TRACE_FT_VERBOSE(X)					    \
    do {							    \
	if (globals.debug & DBG_FT_VERBOSE) {				    \
	    fprintf(stderr, "%s:%d: FT_VERBOSE: ", __FILE__, __LINE__);	    \
	    fprintf X;						    \
	    fprintf(stderr, "\n");				    \
	}							    \
    } while(0)
#define TRACE_GUI(X)						    \
    do {							    \
	if (globals.debug & DBG_GUI) {					    \
	    fprintf(stderr, "%s:%d: GUI: ", __FILE__, __LINE__);    \
	    fprintf X;						    \
	    fprintf(stderr, "\n");				    \
	}							    \
    } while(0)
#define TRACE_EVENTS(X)						    \
    do {							    \
	if (globals.debug & DBG_EVENT) {				    \
	    fprintf(stderr, "%s:%d: EVENT: ", __FILE__, __LINE__);  \
	    fprintf X;						    \
	    fprintf(stderr, "\n");				    \
	}							    \
    } while(0)
#define TRACE_FIND(X)						    \
    do {							    \
	if (globals.debug & DBG_FIND) {				            \
	    fprintf(stderr, "%s:%d: FIND: ", __FILE__, __LINE__);   \
	    fprintf X;						    \
	    fprintf(stderr, "\n");				    \
	}							    \
    } while(0)

#define TRACE_FILES(X)						    \
    do {							    \
	if (globals.debug & DBG_FILES) {				    \
	    fprintf(stderr, "%s:%d: FILES: ", __FILE__, __LINE__);  \
	    fprintf X;						    \
	    fprintf(stderr, "\n");				    \
	}							    \
    } while(0)

#endif /* 0 */

#endif /* XDVI_DEBUG_H_ */
