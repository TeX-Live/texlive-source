/*
  $Id: mkind.h,v 1.2 1999/07/30 08:35:38 kehr Exp $

  This is the minimum excerpt form the original `mkind.h' of the
  makeindex-3 system making the modified version of ordrules compile.

  */

#include <stdio.h>
#include <stdlib.h>

#ifndef _MKIND_H
#define _MKIND_H

#ifndef PROTO
#define PROTO( x ) x
#endif

#if    ANSI_PROTOTYPES
#define ARGS(arg_list)	arg_list
#define VOIDP		void*
#else
#define ARGS(arg_list)	()
#define const
#define VOIDP		char*
#endif

#define SIZE_T  unsigned

#undef TRUE
#define TRUE 1

#undef FALSE
#define FALSE 0

#ifndef STRING_MAX
/* T. Henlich reported a bug that was caused by the 128Byte limit
   here, which was too small for his applications. Stupid thing,
   really took me 1.5 hours to find it ;-(
*/

#define STRING_MAX    1024   /* 128 */
#endif

#define BSH '\\'

#undef NUL
#define NUL '\0'

extern VOIDP fmalloc ARGS((SIZE_T size));

#endif /* _MKIND_H */


/*
  $Log: mkind.h,v $
  Revision 1.2  1999/07/30 08:35:38  kehr
  Lifted length of STRING_MAX to 4096. Caused bugs in Linux as reported
  by T. Henlich.

  Revision 1.1  1996/03/27  20:29:05  kehr
  It works. Today I had my first success in getting the FFI running with
  the ordrules-library. The interface is defined in `ordrulei.lsp' and
  allows direct access to the functions in `ordrules.c'.


  */
