dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([xdvipdfmx], [0.7.8], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([src/dvipdfmx.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([xdvipdfmx])

AC_CONFIG_HEADERS([config.h])

KPSE_LT_HACK

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h inttypes.h sys/types.h sys/wait.h])

dnl Checks for library functions.
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([open close getenv mkstemp basename])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_STRUCT_TM
AC_STRUCT_TIMEZONE
AC_C_BIGENDIAN
AC_TYPE_UINT32_T

AC_CHECK_DECL([timezone],
              [AC_DEFINE([HAVE_TIMEZONE], 1,
                         [Define if <time.h> has timezone as an external variable.])],
              [],
              [[#include <time.h>]])
AC_CHECK_MEMBER([struct tm.tm_gmtoff],
                [AC_DEFINE([HAVE_TM_GMTOFF], 1,
                           [Define if struct tm has tm_gmtoff as a member.])],
                [],
                [[#include <time.h>]])

AC_CHECK_SIZEOF([char])
AC_CHECK_SIZEOF([short])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])

AC_SEARCH_LIBS([pow], [m])

KPSE_KPATHSEA_FLAGS
KPSE_ZLIB_FLAGS
KPSE_LIBPNG_FLAGS
KPSE_FREETYPE2_FLAGS

if test "x$enable_build" != xno || test -f config.force; then

KPSE_ADD_FLAGS([kpathsea])
AC_CHECK_HEADER([kpathsea/version.h], [],
                [AC_MSG_ERROR([Sorry, kpathsea too old])])

KPSE_ADD_FLAGS([zlib])
CHO_CHECK_ZLIB

KPSE_ADD_FLAGS([libpng])
CHO_CHECK_LIBPNG

KPSE_ADD_FLAGS([freetype2])
CHO_CHECK_LIBFREETYPE([xdvipdfmx])

KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

dnl Check for additional libraries.

if test "x$enable_texlive_build" != xyes || test ! -d $srcdir/../kpathsea; then
  CHO_CHECK_LIBPAPER
fi

dnl XXX we should do the check under Mac only
can_build=yes
KPSE_CHECK_FRAMEWORK([ApplicationServices], [ATSFontRef fontRef])
if test "x$kpse_cv_have_ApplicationServices" = xyes; then
  AC_DEFINE([XETEX_MAC], 1, [Define if part of a TeX Live build for Mac OS X.])
fi

if test "x$can_build" = xyes; then
  AC_DEFINE([XETEX], 1, [Define if part of a TeX Live build.])
else
  AC_MSG_WARN([ApplicationServices framework not found.  This version of
xdvipdfmx requires ApplicationServices (on Mac OS X) to be available, otherwise
it cannot support XeTeX output using standard system-installed fonts.])
  if test "x$enable_build" != xno; then
    AC_MSG_ERROR([exiting])
  fi
fi
AM_CONDITIONAL([build], [test "x$can_build" = xyes])
AM_CONDITIONAL([build_MacOsX], [test "x$kpse_cv_have_ApplicationServices" = xyes])

AC_CONFIG_FILES([Makefile src/Makefile])

AC_OUTPUT
