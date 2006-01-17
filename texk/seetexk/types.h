/*
 * Copyright (c) 1987, 1989 University of Maryland
 * Department of Computer Science.  All rights reserved.
 * Permission to copy for any purpose is hereby granted
 * so long as this copyright notice remains intact.
 */

/*
 * MC-TeX types and macros (system dependent).
 * Check to make sure they are correct for your system.
 */

#ifndef _MCTEX_TYPES_
#define _MCTEX_TYPES_

#ifdef WIN32
#include <win32lib.h>
#undef index
#undef DT_RIGHT
#define EndPage DviEndPage
#endif

/*
 * Define BSD_FILE_SYSTEM if you have the BSD file system `stat'
 * structure (with the st_blksize field).  Otherwise, MakeSeekable()
 * will be slower than necessary.
 */
#undef BSD_FILE_SYSTEM

/*
 * Define this as the name of a routine that handles overlapping block
 * copies, if there is such a routine.  Usually it will be called memmove()
 * but on 4.3BSD it is called bcopy().  Note that the 4.2BSD bcopy() does
 * not handle overlap, and must not be used here.  If there is no routine,
 * or if its overlap handling is uncertain, leave BLOCK_COPY undefined.
 *
 * (The bcopy provided in lib/bcopy.c does handle overlap.)
 */
/* #define BLOCK_COPY(from, to, len) memmove(to, from, len) */
#define BLOCK_COPY(from, to, len) bcopy(from, to, len)

#ifdef KPATHSEA

#include <kpathsea/config.h>
#include <kpathsea/c-memstr.h>

#ifdef HAVE_PROTOTYPES
#define NeedFunctionPrototypes 1
#else
#undef NeedFunctionPrototypes
#endif

#else

/*
 * Define void as int if your compiler does not support void,
 * or if there are bugs in its support (e.g., 4.1BSD).
 */
/* #define void int */

/*
 * Define the following if and only if vfprintf is in your C library.
 * If not, lib/error.c will make assumptions about the implementation
 * of stdio.  If neither works, you may have to come up with something
 * yourself.
 */
#define HAVE_VPRINTF

/*
 * If you have memcpy/memmove, but not bcopy, use the definition below.
 * If you have neither, try the bcopy provided in lib/bcopy.c.
 */
#define bcopy(from, to, len) memmove(to, from, len)

/*
 * If you have memcmp, but not bcmp, use the definition below.
 * If you have neither, try the bcmp provided in lib/bcmp.c.
 */
#define bcmp(s1, s2, len) memcmp(s1, s2, len)

#define bzero(s, len) memset(s, '\0', len)

#define index(s, c) strchr(s, c)

#endif /* KPATHSEA */
/*
 * Define the following types and macros as required by your system.
 */

typedef short i16;		/* a 16 bit integer (signed) */

typedef long i32;		/* a 32 bit integer (signed) */
typedef unsigned long ui32;	/* a 32 bit integer (unsigned) */

/* macros to sign extend quantities that are less than 32 bits long */

/* these compilers mishandle (int)(char)(constant), but a subterfuge works */
#if defined(sun) || defined(hp300)
#define Sign8(n)	((i32)(char)(int)(n))
#endif

/* these have signed characters and (int)(char)(constant) works */
#if defined(vax) || defined(mips)
#define	Sign8(n)	((i32)(char)(n))
#endif

/* this works everywhere, but may be slow */
#ifndef	Sign8
#define Sign8(n)	((n) & 0x80 ? ((n) | ~0xff) : (n))
#endif

/* this should work everywhere */
#ifndef Sign16
#define Sign16(n)	((i32)(i16)(n))
#endif

/* this works where int is 32 bits, and >> sign extents and is fast */
#if defined(vax) || defined(mips)
#define	Sign24(n)	(((n) << 8) >> 8)
#endif

/* this works everywhere, but may be slow */
#ifndef	Sign24
#define	Sign24(n)	((n) & 0x800000 ? ((n) | ~0xffffff) : (n))
#endif

/* macros to truncate quantites that may be signed */
#define UnSign8(n)	((i32)(n) & 0xff)
#define UnSign16(n)	((i32)(n) & 0xffff)
#define UnSign24(n)	((i32)(n) & 0xffffff)

#endif /* _MCTEX_TYPES_ */
