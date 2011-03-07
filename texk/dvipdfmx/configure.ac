dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 - 2011 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([dvipdfmx_version], [20110305])[]dnl using unmodified dvipdfmx source tree
AC_INIT([dvipdfmx], dvipdfmx_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([dvipdfmx-]dvipdfmx_version[/src/dvipdfmx.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([dvipdfmx])

AC_CONFIG_HEADERS([config.h])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h inttypes.h sys/types.h])

dnl Checks for library functions.
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([open close system getenv mkstemp basename])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_STRUCT_TM
AC_STRUCT_TIMEZONE
AC_C_BIGENDIAN

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

if test "x$enable_build" != xno || test -f config.force; then

KPSE_ADD_FLAGS([kpathsea])
AC_CHECK_HEADER([kpathsea/version.h], [],
                [AC_MSG_ERROR([Sorry, kpathsea too old])])

KPSE_ADD_FLAGS([zlib])
CHO_CHECK_ZLIB

KPSE_ADD_FLAGS([libpng])
CHO_CHECK_LIBPNG

KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

dnl We always have this in TeX Live
AC_DEFINE([__TDS_VERSION__], [0x200406L],
          [Define to 0x200406L if your libkpathsea supports enc formats.])

dnl Check for additional libraries.

if test "x$enable_texlive_build" != xyes || test ! -d $srdir/../kpathsea; then
  CHO_CHECK_LIBPAPER
fi

AC_SUBST([DVIPDFMX_TREE], [dvipdfmx-]dvipdfmx_version)

KPSE_CHECK_WIN32
AM_CONDITIONAL([WIN32], [test "x$kpse_cv_have_win32" != xno])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
