#ifndef _ZZIP__CONFIG_H
#define _ZZIP__CONFIG_H 1
 
/* Define to 1 if you have the <byteswap.h> header file. */
/* #undef ZZIP_HAVE_BYTESWAP_H  */

/* TH need this to get the proper definition of _mkdir() in write.c */

/* Define to 1 if you have the <direct.h> header file. */
#ifndef ZZIP_HAVE_DIRECT_H
#ifdef WIN32
#define ZZIP_HAVE_DIRECT_H 1
#endif
#endif

/* Define to 1 if you have the <dirent.h> header file, and it defines `DIR'.
   */
#ifndef ZZIP_HAVE_DIRENT_H 
#define ZZIP_HAVE_DIRENT_H  1 
#endif

/* Define to 1 if you have the <dlfcn.h> header file. */
#ifndef ZZIP_HAVE_DLFCN_H 
#define ZZIP_HAVE_DLFCN_H  1 
#endif

/*TH Zziplibs's own __fnmatch.h gives a compiler warning because it
  uses puts() without loading stdio.h, so I prefer not to use it if
  there is a correct system header, even though the warning is
  harmless.
*/

/* Define to 1 if you have the <fnmatch.h> header file. */
#ifndef ZZIP_HAVE_FNMATCH_H
#ifndef WIN32
#define ZZIP_HAVE_FNMATCH_H 1
#endif
#endif

/* Define to 1 if you have the <inttypes.h> header file. */
#ifndef ZZIP_HAVE_INTTYPES_H 
#define ZZIP_HAVE_INTTYPES_H  1 
#endif

/* Define to 1 if you have the <io.h> header file. */
#ifndef ZZIP_HAVE_IO_H
#ifdef WIN32
#define ZZIP_HAVE_IO_H 1
#endif
#endif

/* Define to 1 if you have the <memory.h> header file. */
#ifndef ZZIP_HAVE_MEMORY_H 
#define ZZIP_HAVE_MEMORY_H  1 
#endif

/* Define to 1 if you have the <ndir.h> header file, and it defines `DIR'. */
/* #undef ZZIP_HAVE_NDIR_H */

#if defined __sun__ || defined __sun
/* Define to 1 if you have the <sys/int_types.h> header file. */
#define ZZIP_HAVE_SYS_INT_TYPES_H 1
#elif defined(_AIX)
#else
/* Define to 1 if you have the <stdint.h> header file. */
#define ZZIP_HAVE_STDINT_H 1 
#endif


/* Define to 1 if you have the <stdlib.h> header file. */
#ifndef ZZIP_HAVE_STDLIB_H 
#define ZZIP_HAVE_STDLIB_H  1 
#endif

/* Define to 1 if you have the `strcasecmp' function. */
#ifndef ZZIP_HAVE_STRCASECMP 
#define ZZIP_HAVE_STRCASECMP  1 
#endif

/* Define to 1 if you have the <strings.h> header file. */
#ifndef ZZIP_HAVE_STRINGS_H 
#define ZZIP_HAVE_STRINGS_H  1 
#endif

/* Define to 1 if you have the <string.h> header file. */
#ifndef ZZIP_HAVE_STRING_H 
#define ZZIP_HAVE_STRING_H  1 
#endif

/* Define to 1 if you have the `strndup' function. */
#ifndef ZZIP_HAVE_STRNDUP 
#define ZZIP_HAVE_STRNDUP  1 
#endif

/* Define to 1 if you have the <sys/dir.h> header file, and it defines `DIR'.
   */
/* #undef ZZIP_HAVE_SYS_DIR_H */

/* Define to 1 if you have the <sys/mman.h> header file. */
/* undefe ZZIP_HAVE_SYS_MMAN_H  */

/* Define to 1 if you have the <sys/ndir.h> header file, and it defines `DIR'.
   */
/* #undef ZZIP_HAVE_SYS_NDIR_H */

/* Define to 1 if you have the <sys/param.h> header file. */
#ifndef ZZIP_HAVE_SYS_PARAM_H 
#define ZZIP_HAVE_SYS_PARAM_H  1 
#endif

/* Define to 1 if you have the <sys/stat.h> header file. */
#ifndef ZZIP_HAVE_SYS_STAT_H 
#define ZZIP_HAVE_SYS_STAT_H  1 
#endif

/* Define to 1 if you have the <sys/types.h> header file. */
#ifndef ZZIP_HAVE_SYS_TYPES_H 
#define ZZIP_HAVE_SYS_TYPES_H  1 
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef ZZIP_HAVE_UNISTD_H 
#define ZZIP_HAVE_UNISTD_H  1 
#endif

/* Define to 1 if you have the <winbase.h> header file. */
/* #undef ZZIP_HAVE_WINBASE_H */

/* Define to 1 if you have the <windows.h> header file. */
/* #undef ZZIP_HAVE_WINDOWS_H */

/* Define to 1 if you have the <winnt.h> header file. */
/* #undef ZZIP_HAVE_WINNT_H */

/* Define to 1 if you have the <zlib.h> header file. */
#ifndef ZZIP_HAVE_ZLIB_H 
#define ZZIP_HAVE_ZLIB_H  1 
#endif

/* whether the system defaults to 32bit off_t but can do 64bit when requested
   */
#ifndef ZZIP_LARGEFILE_SENSITIVE 
#define ZZIP_LARGEFILE_SENSITIVE  1 
#endif

/* Name of package */
#ifndef ZZIP_PACKAGE 
#define ZZIP_PACKAGE  "zziplib" 
#endif

/* Define to the address where bug reports for this package should be sent. */
#ifndef ZZIP_PACKAGE_BUGREPORT 
#define ZZIP_PACKAGE_BUGREPORT  "" 
#endif

/* Define to the full name of this package. */
#ifndef ZZIP_PACKAGE_NAME 
#define ZZIP_PACKAGE_NAME  "" 
#endif

/* Define to the full name and version of this package. */
#ifndef ZZIP_PACKAGE_STRING 
#define ZZIP_PACKAGE_STRING  "" 
#endif

/* Define to the one symbol short name of this package. */
#ifndef ZZIP_PACKAGE_TARNAME 
#define ZZIP_PACKAGE_TARNAME  "" 
#endif

/* Define to the version of this package. */
#ifndef ZZIP_PACKAGE_VERSION 
#define ZZIP_PACKAGE_VERSION  "" 
#endif

/* The number of bytes in type int */
#ifndef ZZIP_SIZEOF_INT 
#define ZZIP_SIZEOF_INT  4 
#endif

/* The number of bytes in type long */
#ifndef ZZIP_SIZEOF_LONG 
#define ZZIP_SIZEOF_LONG  4 
#endif

/* The number of bytes in type short */
#ifndef ZZIP_SIZEOF_SHORT 
#define ZZIP_SIZEOF_SHORT  2 
#endif

/* Define to 1 if you have the ANSI C header files. */
#ifndef ZZIP_STDC_HEADERS 
#define ZZIP_STDC_HEADERS  1 
#endif

/* Version number of package */
#ifndef ZZIP_VERSION 
#define ZZIP_VERSION  "0.13.47" 
#endif

/* Define to 1 if your processor stores words with the most significant byte
   first (like Motorola and SPARC, unlike Intel and VAX). */
/* #undef ZZIP_WORDS_BIGENDIAN */

/* Number of bits in a file offset, on hosts where this is settable. */
#ifndef ZZIP__FILE_OFFSET_BITS 
#define ZZIP__FILE_OFFSET_BITS  64 
#endif

/* Define for large files, on AIX-style hosts. */
/* #undef ZZIP__LARGE_FILES */

/* Define to `long long' if <sys/types.h> does not define. */
#ifndef ZZIP___int64 
#define ZZIP___int64  long long 
#endif

/* Define to empty if `const' does not conform to ANSI C. */
/* #undef _zzip_const */

/* Define to `__inline__' or `__inline' if that's what the C compiler
   calls it, or to nothing if 'inline' is not supported under any name.  */
#ifndef __cplusplus
/* #undef _zzip_inline */
#endif

/* Define to `_zzip_off_t' if <sys/types.h> does not define. */
#ifndef _zzip_off64_t 
#define _zzip_off64_t  _zzip_off_t 
#endif

/* Define to `long' if <sys/types.h> does not define. */
/* #undef _zzip_off_t */

/* Define to equivalent of C99 restrict keyword, or to nothing if this is not
   supported. Do not define if restrict is supported directly. */
#ifndef _zzip_restrict 
#define _zzip_restrict
#endif

/* Define to `unsigned' if <sys/types.h> does not define. */
/* #undef _zzip_size_t */

/* Define to `int' if <sys/types.h> does not define. */
/* #undef _zzip_ssize_t */
 
/* once: _ZZIP__CONFIG_H */
#endif
