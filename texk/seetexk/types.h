/* Copyright (c) 1987, 1989, 2012 University of Maryland Department of
   Computer Science.
   
   Permission is hereby granted, free of charge, to any person obtaining
   a copy of this software and associated documentation files (the
   "Software"), to deal in the Software without restriction, including
   without limitation, the rights to use, copy, modify, merge, publish,
   distribute, sublicense, and/or sell copies of the Software, and to
   permit persons to whom the Software is furnished to do so, subject to
   the following conditions: The above copyright notice, this permission
   notice and the disclaimer statement shall be included in all copies
   or substantial portions of the Software.
   
   THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
   EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
   MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
   IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
   CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
   TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
   SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

/*
 * MC-TeX types and macros (system dependent).
 * Check to make sure they are correct for your system.
 */

#ifndef _MCTEX_TYPES_
#define _MCTEX_TYPES_

#ifdef KPATHSEA

#include <kpathsea/config.h>
#include <kpathsea/c-memstr.h>

#define BLOCK_COPY(from, to, len) memmove(to, from, len)

#else

#if defined(WIN32) && !defined(__MINGW32__)
#include <win32lib.h> /* not KPATHSEA */
#undef index
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
#define BLOCK_COPY(from, to, len) memmove(to, from, len)

/*
 * Define void as int if your compiler does not support void,
 * or if there are bugs in its support (e.g., 4.1BSD).
 */
/* #define void int */

#endif /* KPATHSEA */

/*
 * Conflicting WIN32 declarations.
 */
#undef DT_RIGHT
#define EndPage DviEndPage

/*
 * Define the following types and macros as required by your system.
 */

typedef int16_t i16;		/* a 16 bit integer (signed) */

typedef int32_t i32;		/* a 32 bit integer (signed) */
typedef uint32_t ui32;		/* a 32 bit integer (unsigned) */

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
