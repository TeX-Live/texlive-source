/* config.h.  Edited for MiKTeX.  */

/************************************************************************

  Part of the dvipng distribution

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU Lesser General Public License as
  published by the Free Software Foundation, either version 3 of the
  License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this program. If not, see
  <http://www.gnu.org/licenses/>.

  Copyright (C) 2002-2015 Jan-Åke Larsson

************************************************************************/

/* Define to one of `_getb67', `GETB67', `getb67' for Cray-2 and Cray-YMP
   systems. This function is required for `alloca.c' support on those systems.
   */
#undef CRAY_STACKSEG_END

/* Define to 1 if using `alloca.c'. */
#undef C_ALLOCA

/* Define as 1 to get the debug (-d) option. */
#define DEBUG 1

/* The environment setting for $SELFAUTODIR */
#undef ENV_SELFAUTODIR

/* The environment setting for $SELFAUTOLOC */
#undef ENV_SELFAUTOLOC

/* The environment setting for $SELFAUTOPARENT */
#undef ENV_SELFAUTOPARENT

/* Define as the path to GhostScript. */
#undef GS_PATH

/* Define to 1 if you have `alloca', as a function or macro. */
#define HAVE_ALLOCA 1

/* Define to 1 if you have <alloca.h> and it should be used (not on Ultrix).
   */
#undef HAVE_ALLOCA_H

/* Define to 1 if you don't have `vprintf' but do have `_doprnt.' */
#undef HAVE_DOPRNT

/* Define to 1 if you have the `dup2' function. */
#undef HAVE_DUP2

/* Define to 1 if you have the <fcntl.h> header file. */
#define HAVE_FCNTL_H 1

/* Define to 1 if you have the `fork' function. */
#undef HAVE_FORK

/* Define to 1 if you have freetype2 */
#undef HAVE_FT2

/* Define to 1 if you have the `ftime' function. */
#undef HAVE_FTIME

/* Define to 1 if you have the `FT_Library_Version' function. */
#undef HAVE_FT_LIBRARY_VERSION

/* Define to 1 if you have the `gdImageAlphaBlending' function. */
#undef HAVE_GDIMAGEALPHABLENDING

/* Define to 1 if you have the `gdImageColorResolveAlpha' function. */
#undef HAVE_GDIMAGECOLORRESOLVEALPHA

/* Define to 1 if you have the `gdImageCreateTrueColor' function. */
#define HAVE_GDIMAGECREATETRUECOLOR 1

/* Define to 1 if you have the `gdImageGif' function. */
#undef HAVE_GDIMAGEGIF

/* Define to 1 if you have the `gdImagePngEx' function. */
#define HAVE_GDIMAGEPNGEX 1

/* Define to 1 if you have the `gdImageTrueColorToPalette' function. */
#define HAVE_GDIMAGETRUECOLORTOPALETTE 1

/* Define to 1 if you have the <gd.h> header file. */
#define HAVE_GD_H 1

/* Define to 1 if you have the `getpagesize' function. */
#undef HAVE_GETPAGESIZE

/* Define to 1 if you have the `gettimeofday' function. */
#undef HAVE_GETTIMEOFDAY

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <kpathsea/kpathsea.h> header file. */
#define HAVE_KPATHSEA_KPATHSEA_H 1

/* Define to 1 if your kpathsea has kpse_enc_format */
#define HAVE_KPSE_ENC_FORMATS 1

/* Define to 1 if you have the `gd' library (-lgd). */
#define HAVE_LIBGD 1

/* Define to 1 if you have the `gen' library (-lgen). */
#undef HAVE_LIBGEN

/* Define to 1 if you have the `kpathsea' library (-lkpathsea). */
#define HAVE_LIBKPATHSEA 1

/* Define to 1 if you have the `m' library (-lm). */
#define HAVE_LIBM 1

/* Define to 1 if you have the `png' library (-lpng). */
#define HAVE_LIBPNG 1

/* Define to 1 if you have the `z' library (-lz). */
#undef HAVE_LIBZ

/* Define to 1 if your system has a GNU libc compatible `malloc' function, and
   to 0 otherwise. */
#undef HAVE_MALLOC

/* Define to 1 if you have the <memory.h> header file. */
#undef HAVE_MEMORY_H

/* Define to 1 if you have the `memset' function. */
#define HAVE_MEMSET 1

/* Define to 1 if you have a working `mmap' system call. */
#undef HAVE_MMAP

/* Define to 1 if you have the `munmap' function. */
#undef HAVE_MUNMAP

/* Define to 1 if you have the <png.h> header file. */
#define HAVE_PNG_H 1

/* Define to 1 if you have the `pow' function. */
#undef HAVE_POW

/* Define to 1 if you have the `putenv' function. */
#undef HAVE_PUTENV

/* Define to 1 if stdbool.h conforms to C99. */
#undef HAVE_STDBOOL_H 1

/* Define to 1 if you have the <stdint.h> header file. */
#undef HAVE_STDINT_H

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the `strchr' function. */
#define HAVE_STRCHR 1

/* Define to 1 if you have the <strings.h> header file. */
#undef HAVE_STRINGS_H

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the `strrchr' function. */
#undef HAVE_STRRCHR

/* Define to 1 if you have the `strstr' function. */
#undef HAVE_STRSTR

/* Define to 1 if you have the `strtol' function. */
#undef HAVE_STRTOL

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/time.h> header file. */
#undef HAVE_SYS_TIME_H

/* Define to 1 if you have the <sys/types.h> header file. */
#undef HAVE_SYS_TYPES_H

/* Define to 1 if you have <sys/wait.h> that is POSIX.1 compatible. */
#undef HAVE_SYS_WAIT_H

/* Define to 1 if you have the <unistd.h> header file. */
#undef HAVE_UNISTD_H

/* Define to 1 if you have the `vfork' function. */
#undef HAVE_VFORK

/* Define to 1 if you have the <vfork.h> header file. */
#undef HAVE_VFORK_H

/* Define to 1 if you have the `vprintf' function. */
#define HAVE_VPRINTF 1

/* Define to 1 if `fork' works. */
#undef HAVE_WORKING_FORK

/* Define to 1 if `vfork' works. */
#undef HAVE_WORKING_VFORK

/* Define to 1 if the system has the type `_Bool'. */
#undef HAVE__BOOL 1

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT "dvipng@nongnu.org"

/* Define to the full name of this package. */
#define PACKAGE_NAME "dvipng"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "dvipng 1.7"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "dvipng"

/* Define to the version of this package. */
#define PACKAGE_VERSION "1.7"

/* If using the C implementation of alloca, define if you know the
   direction of stack growth for your system; otherwise it will be
   automatically deduced at run-time.
	STACK_DIRECTION > 0 => grows toward higher addresses
	STACK_DIRECTION < 0 => grows toward lower addresses
	STACK_DIRECTION = 0 => direction of growth unknown */
#undef STACK_DIRECTION

/* Define to 1 if you have the ANSI C header files. */
#undef STDC_HEADERS

/* Define to 1 if you can safely include both <sys/time.h> and <time.h>. */
#undef TIME_WITH_SYS_TIME

/* Define as 1 to get execution time output. */
#undef TIMING

/* Define to empty if `const' does not conform to ANSI C. */
#undef const

/* Define to `long long' if <inttypes.h> does not define it. */
#undef int64_t

/* Define to rpl_malloc if the replacement function should be used. */
#undef malloc

/* Define to `int' if <sys/types.h> does not define. */
#undef pid_t

/* Define to `unsigned' if <sys/types.h> does not define. */
#undef size_t

/* Define to `unsigned long long' if <inttypes.h> does not define it. */
#undef uint64_t

/* Define as `fork' if `vfork' does not work. */
#undef vfork
