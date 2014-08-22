/* config.h: master configuration file, included first by all compilable
   source files (not headers).

   Copyright 1993, 1995, 1996, 1997, 2008, 2010, 2011 Karl Berry.
   Copyright 2000, 2003, 2004, 2005 Olaf Weber.

   This library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Lesser General Public
   License as published by the Free Software Foundation; either
   version 2.1 of the License, or (at your option) any later version.

   This library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Lesser General Public License for more details.

   You should have received a copy of the GNU Lesser General Public License
   along with this library; if not, see <http://www.gnu.org/licenses/>.  */

#ifndef KPATHSEA_CONFIG_H
#define KPATHSEA_CONFIG_H

/* System defines are for non-Unix systems only.  (Testing for all Unix
   variations should be done in configure.)  Presently the defines used
   are: AMIGA DOS OS2 WIN32.  I do not use any of these systems myself;
   if you do, I'd be grateful for any changes.  */

#if defined(DJGPP)    || defined(__DJGPP__)     || \
    defined(CYGWIN)   || defined(__CYGWIN__)    || \
    defined(CYGWIN32) || defined(__CYGWIN32__)  || \
    defined(MINGW32)  || defined(__MINGW32__)
#define __i386_pc_gnu__
#endif

/* If we have either DOS or OS2, we are DOSISH.  Cygwin pretends to be
   Unix, mostly, so don't include it here.  */
#if defined(OS2)     || \
    defined(MSDOS)   || defined(__MSDOS__) || defined(DOS)    || \
    defined(WIN32)   || defined(__WIN32__) || defined(_WIN32) || \
    defined(DJGPP)   || defined(__DJGPP__) || \
    defined(MINGW32) || defined(__MINGW32__)
#define DOSISH
#endif

/* case-insensitive filename comparisons? */
#if defined (DOSISH)
#define MONOCASE_FILENAMES
#endif

/* NULL device. */
#if defined (DOSISH)
#define DEV_NULL "NUL"
#else
#define DEV_NULL "/dev/null"
#endif

#if defined (WIN32) && !defined (__STDC__)
#define __STDC__ 1
#endif

/* System dependencies that are figured out by `configure'.  */
#include <kpathsea/c-auto.h>

#ifdef __DJGPP__
#include <fcntl.h>      /* for long filenames' stuff */
#include <dir.h>        /* for `getdisk' */
#include <io.h>         /* for `setmode' */
#endif

/* Some drivers have partially integrated kpathsea changes.  */
#ifndef KPATHSEA
#define KPATHSEA 34
#endif

#ifdef __MINGW32__
/* In mingw32, the eof() function is part of the !_NO_OLDNAMES section
   of <io.h>, that is read in automatically via <unistd.h>. We cannot
   allow that because web2c/lib/eofeoln.c defines a private,
   incompatible function named eof().
   But many of the other things defined via !_NO_OLDNAMES are needed,
   so #define _NO_OLDNAMES cannot be used. So, temporarily define eof
   as a macro.
*/
#define eof saved_eof
#include <kpathsea/c-std.h>    /* <stdio.h>, <math.h>, etc.  */
#undef eof
#else
#include <kpathsea/c-std.h>    /* <stdio.h>, <math.h>, etc.  */
#endif

#include <kpathsea/c-proto.h>  /* KPSEDLL.  */

/*
  This must be included after "c-proto.h"
  but before "lib.h". FP.
*/
#if defined (WIN32) || defined (_WIN32)
#include <kpathsea/knj.h>
#ifdef __MINGW32__
#include <kpathsea/mingw32.h>
#else
#include <kpathsea/win32lib.h>
#endif
#endif

#if defined(WIN32) || defined(WRAP_SNPRINTF)
/* All Unix systems known to us have snprintf() and vsnprintf(),
   while all known Windows systems have _snprintf() and _vsnprintf().
   
   Consider a call
     RET = snprintf(BUF, SIZE, FMT, ...)
   and let LEN be the number that would be written to BUF if SIZE were
   sufficiently large (not counting the trailing null byte).

   C99 requires that snprintf
   (A) modifies at most the first SIZE bytes of BUF,
   (B) writes a trailing null byte except when SIZE=0, and
   (C) always returns LEN.
   
   All known implementations (except some ancient buggy ones, e.g., for
   64-bit Solaris 7 from Oct. 1998) satisfy (A).  As long as LEN<SIZE
   they all write a trailing null byte and return LEN.

   Condition (C) is, however, violated for LEN>=SIZE by some older
   implementations (SUSv2, glibc <= 2.0.6, etc.) and for Windows even
   (B) is violated..

   TeX Live does not require the full C99 semantics, but will need that
   (1) there is always a trailing null byte, and
   (2) for LEN>=SIZE the return value is either >=SIZE or <0, i.e.,
       (unsigned)RET >= (unsigned)SIZE.

   A violation of (2) is detected by configure (except when cross
   compiling) and by a runtime check in the initialization routine
   kpathsea_set_program_name.
   
   A violation of (1) is handled here through static inline wrapper
   functions.  */


#undef snprintf
#undef vsnprintf

#ifdef __cplusplus
extern "C" {
#endif

static inline int
kpse_vsnprintf (char *str, size_t size, const char *format, va_list ap)
{
#ifdef WIN32
  int ret = _vsnprintf (str, size, format, ap);
#else
  int ret = vsnprintf (str, size, format, ap);
#endif
  if (size > 0 && (unsigned)ret >= (unsigned)size)
    str [size - 1] = '\0';
  return ret;
}

static inline int
kpse_snprintf (char *str, size_t size, const char *format, ...)
{
  int ret;
  va_list ap;

  va_start (ap, format);
  ret = kpse_vsnprintf (str, size, format, ap);
  va_end (ap);
  return ret;
}

#ifdef __cplusplus
}
#endif

#define snprintf kpse_snprintf
#define vsnprintf kpse_vsnprintf

#endif /* WIN32 || WRAP_SNPRINTF */

/* Transform filename characters for use in hash tables.  */
#if defined(MONOCASE_FILENAMES)
#if defined(WIN32) && !defined(__i386_pc_gnu__)
/* This is way faster under Win32. */
#define TRANSFORM(x) ((unsigned)CharLower((LPTSTR)(BYTE)(x)))
#else
#define TRANSFORM(x) (tolower(x))
#endif
#else
#define TRANSFORM(x) (x)
#endif

#include <kpathsea/debug.h>    /* Runtime tracing.  */
#include <kpathsea/lib.h>      /* STREQ, etc. */
#include <kpathsea/types.h>    /* <sys/types.h>, boolean, string, etc. */
#include <kpathsea/progname.h> /* for kpse_invocation_*name */


/* If you want to find subdirectories in a directory with non-Unix
   semantics (specifically, if a directory with no subdirectories does
   not have exactly two links), define this.  */
#if !defined (VMS) && !defined (VMCMS)
#if !defined (DOSISH) || defined(__DJGPP__)
/* Surprise!  DJGPP returns st_nlink exactly like on Unix.  */
#define ST_NLINK_TRICK
#endif /* either not DOSISH or __DJGPP__ */
#endif /* not DOS and not VMS and not VMCMS */

#ifdef AMIGA
/* No popen/pclose on Amiga, but rather than put #ifdef's in tex-make.c,
   let's get rid of the functions here.  (CallMF will automatically
   generate fonts.)  pclose must not be simply empty, since it still
   occurs in a comparison.  */
#define popen(cmd, mode) NULL
#define pclose(file) 0
#endif /* AMIGA */

#ifdef OS2
#define access ln_access
#define chmod ln_chmod
#define creat ln_creat
#define fopen ln_fopen
#define freopen ln_freopen
#define lstat ln_lstat
#define open ln_open
#define remove ln_remove
#define rename ln_rename
#define sopen ln_sopen
#define stat ln_stat
#define unlink ln_unlink
#endif /* OS2 */

#endif /* not KPATHSEA_CONFIG_H */
