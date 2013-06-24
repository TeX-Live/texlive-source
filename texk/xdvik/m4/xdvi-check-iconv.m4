# Autoconf macros for xdvik.
# Copyright (C) 2004-2009 Stefan Ulrich <xdvi-core@lists.sourceforge.net>
# Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# XDVI_CHECK_ICONV
# ----------------
# Check for <iconv.h>, iconv(), and for type of iconv() input argument.
AC_DEFUN([XDVI_CHECK_ICONV], [dnl
AC_ARG_WITH([iconv],
            AS_HELP_STRING([--with-iconv],
                           [Use an iconv library @<:@default=check@:>@]),
            [],
            [with_iconv=check])[]dnl
dnl
AC_ARG_WITH([iconv-include],
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
dnl
AS_IF([test "x$with_iconv" != xno], [dnl
  xdvi_iconv_save_CPPFLAGS=$CPPFLAGS
  AC_CHECK_HEADERS([iconv.h], [dnl
    # Check if -liconv or -lrecode is needed for iconv()
    _XDVI_ICONV_LIB
    if test "x$xdvi_cv_search_iconv" != xno; then
      if test "x$xdvi_cv_search_iconv" = "xnone required"; then
        iconv_libs=
      else
        iconv_libs=$xdvi_cv_search_iconv
      fi
      AC_DEFINE([HAVE_ICONV], 1,
                [Define to 1 if you have the `iconv' function.])
      _XDVI_ICONV_CHAR_PPTR_TYPE
    fi], [], [AC_INCLUDES_DEFAULT])
  CPPFLAGS=$xdvi_iconv_save_CPPFLAGS
])[]dnl
AC_SUBST([iconv_includes])
AC_SUBST([iconv_libpath])
AC_SUBST([iconv_libs])
]) # XDVI_CHECK_ICONV

# _XDVI_ICONV_LIB
# ---------------
# Check for library containing iconv(), could be -liconv or -lrecode.
# Much like AC_SEARCH_LIBS([iconv], [iconv recode]),
# but needs to '#include <iconv.h>'.
m4_define([_XDVI_ICONV_LIB], [dnl
AC_CACHE_CHECK([for library containing iconv],
               [xdvi_cv_search_iconv], [dnl
xdvi_iconv_save_LDFLAGS=$LDFLAGS
xdvi_iconv_save_LIBS=$LIBS
LDFLAGS="$iconv_libpath $LDFLAGS"
AC_LANG_CONFTEST([AC_LANG_PROGRAM([[#include <stdlib.h>
#include <iconv.h>]],
                                  [[iconv_t cd = iconv_open("","");]])])
xdvi_cv_search_iconv=no
for xdvi_lib in "" -liconv -lrecode; do
  LIBS="$xdvi_lib $xdvi_iconv_save_LIBS"
  AC_LINK_IFELSE([],
                 [xdvi_cv_search_iconv=$xdvi_lib
                  break])
done
test "x$xdvi_cv_search_iconv" = x && xdvi_cv_search_iconv="none required"
LDFLAGS=$xdvi_iconv_save_LDFLAGS
LIBS=$xdvi_iconv_save_LIBS])
]) # _XDVI_ICONV_LIB

# _XDVI_ICONV_CHAR_PPTR_TYPE
# --------------------------
# Check whether iconv takes a 'const char **' or a 'char **' input argument.
# According to IEEE 1003.1, `char **' is correct, but e.g. librecode
# uses `const char **'.
# Inspired by Autoconf's AC_FUNC_SELECT_ARGTYPES we do this without the need
# to run a test program or to use C++.
m4_define([_XDVI_ICONV_CHAR_PPTR_TYPE], [dnl
AC_CACHE_CHECK([for iconv input type],
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
]) # _XDVI_ICONV_CHAR_PPTR_TYPE
