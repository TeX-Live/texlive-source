# Autoconf macros for xdvik.
# Copyright (C) 2004 - 2009 Stefan Ulrich <xdvi-core@lists.sourceforge.net>
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_ICONV_CHAR_PPTR_TYPE
# -------------------------
# Check whether iconv takes a 'const char **' or a 'char **' input argument.
# According to IEEE 1003.1, `char **' is correct, but e.g. librecode
# uses `const char **'.
# Inspired by Autoconf's AC_FUNC_SELECT_ARGTYPES we do this without the need
# to run a test program or to use C++.
AC_DEFUN([XDVI_ICONV_CHAR_PPTR_TYPE],
[AC_CACHE_CHECK([for iconv input type],
                [xdvi_cv_iconv_char_pptr_type],
                [
  xdvi_iconv_save_cppflags=$CPPFLAGS
  xdvi_iconv_save_libs=$LIBS
  CPPFLAGS="$CPPFLAGS $iconv_includes"
  LIBS="$LIBS $iconv_libpath $iconv_libs"
  AC_COMPILE_IFELSE(
    [AC_LANG_PROGRAM(
      [[
/* iconv() definitions may differ depending on following macros ... */
#ifdef __hpux
/* On HP-UX 10.10 B and 20.10, compiling with _XOPEN_SOURCE + ..._EXTENDED
 * leads to poll() not realizing that a file descriptor is writable in psgs.c.
 */
# define _HPUX_SOURCE	1
#else
# ifndef _XOPEN_SOURCE
# define _XOPEN_SOURCE	600
# endif
# define _XOPEN_SOURCE_EXTENDED	1
# define __EXTENSIONS__	1	/* needed to get struct timeval on SunOS 5.5 */
# define _SVID_SOURCE	1	/* needed to get S_IFLNK in glibc */
# define _BSD_SOURCE	1	/* needed to get F_SETOWN in glibc-2.1.3 */
#endif

#include <iconv.h>
      ]],
      [[extern size_t iconv(iconv_t, char **, size_t *, char**, size_t*);]])],
    [xdvi_cv_iconv_char_pptr_type='char **'],
    [xdvi_cv_iconv_char_pptr_type='const char **'])
  CPPFLAGS=$xdvi_iconv_save_cppflags
  LIBS=$xdvi_iconv_save_libs])
AC_DEFINE_UNQUOTED([ICONV_CHAR_PPTR_TYPE], [$xdvi_cv_iconv_char_pptr_type],
                   [Define the type of the iconv input string (char ** or const char **)])
]) # XDVI_ICONV_CHAR_PPTR_TYPE
