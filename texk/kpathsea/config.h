/* config.h: master configuration file, included first by all compilable
   source files (not headers).

   Copyright 1993, 1995, 1996, 1997, 2008, 2010 Karl Berry.
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

#include <kpathsea/c-proto.h>  /* Macros to discard or keep prototypes.  */

/*
  This must be included after "c-proto.h"
  but before "lib.h". FP.
*/
#if defined (WIN32) || defined (_WIN32)
#ifdef __MINGW32__
#include <kpathsea/mingw32.h>
#else
#include <kpathsea/win32lib.h>
#endif
#endif

#include <kpathsea/debug.h>    /* Runtime tracing.  */
#include <kpathsea/lib.h>      /* STREQ, etc. */
#include <kpathsea/types.h>    /* <sys/types.h>, boolean, string, etc. */
#include <kpathsea/progname.h> /* for program_invocation_*name */


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
