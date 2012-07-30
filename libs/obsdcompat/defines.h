/*
 * Copyright (c) 1999-2003 Damien Miller.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * ---------------------------------------------------------------------------
 *
 * Modified (butchered) 2006 by Martin Schröder for pdfTeX
 * Modified 2009 by Peter Breitenlohner for new TeX Live build system
 */

#ifndef _DEFINES_H
#define _DEFINES_H

/* $Id: defines.h,v 1.130 2005/12/17 11:04:09 dtucker Exp $ */


/* Types */

/* If sys/types.h does not supply intXX_t, supply them ourselves */
/* (or die trying) */


#ifndef HAVE_U_INT
typedef unsigned int u_int;
#endif

#ifndef HAVE_INTXX_T
# if (SIZEOF_CHAR == 1)
typedef char int8_t;
# else
#  error "8 bit int type not found."
# endif
# if (SIZEOF_SHORT_INT == 2)
typedef short int int16_t;
# else
#  ifdef _UNICOS
#   if (SIZEOF_SHORT_INT == 4)
typedef short int16_t;
#   else
typedef long  int16_t;
#   endif
#  else
#   error "16 bit int type not found."
#  endif /* _UNICOS */
# endif
# if (SIZEOF_INT == 4)
typedef int int32_t;
# else
#  ifdef _UNICOS
typedef long  int32_t;
#  else
#   error "32 bit int type not found."
#  endif /* _UNICOS */
# endif
#endif

/* If sys/types.h does not supply u_intXX_t, supply them ourselves */
#ifndef HAVE_U_INTXX_T
# ifdef HAVE_UINTXX_T
typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;
# define HAVE_U_INTXX_T 1
# else
#  if (SIZEOF_CHAR == 1)
typedef unsigned char u_int8_t;
#  else
#   error "8 bit int type not found."
#  endif
#  if (SIZEOF_SHORT_INT == 2)
typedef unsigned short int u_int16_t;
#  else
#   ifdef _UNICOS
#    if (SIZEOF_SHORT_INT == 4)
typedef unsigned short u_int16_t;
#    else
typedef unsigned long  u_int16_t;
#    endif
#   else
#    error "16 bit int type not found."
#   endif
#  endif
#  if (SIZEOF_INT == 4)
typedef unsigned int u_int32_t;
#  else
#   ifdef _UNICOS
typedef unsigned long  u_int32_t;
#   else
#    error "32 bit int type not found."
#   endif
#  endif
# endif
#define __BIT_TYPES_DEFINED__
#endif

/* 64-bit types */
#ifndef HAVE_INT64_T
# if (SIZEOF_LONG_INT == 8)
typedef long int int64_t;
# else
#  if (SIZEOF_LONG_LONG_INT == 8)
typedef long long int int64_t;
#  endif
# endif
#endif
#ifndef HAVE_U_INT64_T
# if (SIZEOF_LONG_INT == 8)
typedef unsigned long int u_int64_t;
# else
#  if (SIZEOF_LONG_LONG_INT == 8)
typedef unsigned long long int u_int64_t;
#  endif
# endif
#endif

#ifndef HAVE_U_CHAR
typedef unsigned char u_char;
# define HAVE_U_CHAR
#endif /* HAVE_U_CHAR */

#ifndef SIZE_T_MAX
#define SIZE_T_MAX ULONG_MAX
#endif /* SIZE_T_MAX */

#ifndef HAVE_SIZE_T
typedef unsigned int size_t;
# define HAVE_SIZE_T
# define SIZE_T_MAX UINT_MAX
#endif /* HAVE_SIZE_T */

#ifndef HAVE_SSIZE_T
typedef int ssize_t;
# define HAVE_SSIZE_T
#endif /* HAVE_SSIZE_T */

/* Macros */

#ifndef MAX
# define MAX(a,b) (((a)>(b))?(a):(b))
# define MIN(a,b) (((a)<(b))?(a):(b))
#endif

#if !defined(__GNUC__) || (__GNUC__ < 2)
# define __attribute__(x)
#endif /* !defined(__GNUC__) || (__GNUC__ < 2) */

#ifndef __dead
# define __dead	__attribute__((noreturn))
#endif

#if !defined(HAVE_ATTRIBUTE__SENTINEL__) && !defined(__sentinel__)
# define __sentinel__
#endif

#if !defined(HAVE_ATTRIBUTE__BOUNDED__) && !defined(__bounded__)
# define __bounded__(x, y, z)
#endif

/* Function replacement / compatibility hacks */

#if !defined(HAVE___func__) && defined(HAVE___FUNCTION__)
#  define __func__ __FUNCTION__
#elif !defined(HAVE___func__)
#  define __func__ ""
#endif

#if defined(__Lynx__)
 /*
  * LynxOS defines these in param.h which we do not want to include since
  * it will also pull in a bunch of kernel definitions.
  */
  /* Missing prototypes on LynxOS */
  int snprintf (char *, size_t, const char *, ...);
#endif

/* some system headers on HP-UX define YES/NO */
#ifdef YES
# undef YES
#endif
#ifdef NO
# undef NO
#endif

#endif /* _DEFINES_H */
