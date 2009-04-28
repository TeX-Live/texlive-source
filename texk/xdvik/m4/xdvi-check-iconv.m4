# Autoconf macros for xdvik.
# Copyright (C) 2004 - 2009 Stefan Ulrich <xdvi-core@lists.sourceforge.net>
#               ????    or maybe earlier Copyright by Paul Vojta ??
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_CHECK_ICONV
# ----------------
# Check for iconv(), <iconv.h>, and for type of iconv() input argument.
AC_DEFUN([XDVI_CHECK_ICONV],
[AC_ARG_WITH([iconv-include],
             AS_HELP_STRING([--with-iconv-include=DIR],
                            [Specify the location of <iconv.h> header]))[]dnl
AC_ARG_WITH([iconv-libdir],
            AS_HELP_STRING([--with-iconv-libdir=DIR],
                           [Specify the location of iconv (-liconv or -lrecode) library]))[]dnl
AS_CASE([$with_iconv_include],
        [yes | no | ""], [iconv_includes=],
        [iconv_includes="-I$with_iconv_include"])
AS_CASE([$with_iconv_libdir],
        [yes | no | ""], [iconv_libpath=],
        [iconv_libpath="-L$with_iconv_libdir"])
xdvi_iconv_save_CPPFLAGS=$CPPFLAGS
xdvi_iconv_save_LDFLAGS=$LDFLAGS
xdvi_iconv_save_LIBS=$LIBS
CPPFLAGS="$iconv_includes $CPPFLAGS"
LDFLAGS="$iconv_libpath $LDFLAGS"
# Check if -liconv or -lrecode is needed for iconv()
AC_SEARCH_LIBS([iconv], [iconv recode])
AS_CASE([$ac_cv_search_iconv],
        [no | "none required"], [:],
        [iconv_libs=$ac_cv_search_iconv])
AC_CHECK_FUNCS([iconv])
AC_CHECK_HEADERS([iconv.h])
if test "x$ac_cv_func_iconv:$ac_cv_header_iconv_h" = xyes:yes; then
  XDVI_ICONV_CHAR_PPTR_TYPE
fi
CPPFLAGS=$xdvi_iconv_save_CPPFLAGS
LDFLAGS=$xdvi_iconv_save_LDFLAGS
LIBS=$xdvi_iconv_save_LIBS
AC_SUBST([iconv_includes])
AC_SUBST([iconv_libpath])
AC_SUBST([iconv_libs])
]) # XDVI_CHECK_ICONV

# XDVI_ICONV_CHAR_PPTR_TYPE
# -------------------------
# Check whether iconv takes a 'const char **' or a 'char **' input argument.
# According to IEEE 1003.1, `char **' is correct, but e.g. librecode
# uses `const char **'.
# Inspired by Autoconf's AC_FUNC_SELECT_ARGTYPES we do this without the need
# to run a test program or to use C++.
m4_define([XDVI_ICONV_CHAR_PPTR_TYPE],
[AC_CACHE_CHECK([for iconv input type],
                [xdvi_cv_iconv_char_pptr_type],
   [AC_COMPILE_IFELSE(
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
      [xdvi_cv_iconv_char_pptr_type='const char **'])])
AC_DEFINE_UNQUOTED([ICONV_CHAR_PPTR_TYPE], [$xdvi_cv_iconv_char_pptr_type],
                   [Define the type of the iconv input string (char ** or const char **)])
]) # XDVI_ICONV_CHAR_PPTR_TYPE
