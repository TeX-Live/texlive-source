dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2016-2020 Karl Berry <tex-live@tug.org>
dnl   Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([dvipdfm-x (TeX Live)], [20200627], [dvipdfmx@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([agl.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([dvipdfm-x])

AC_CONFIG_HEADERS([config.h])

dnl Checks for header files.
AC_CHECK_HEADERS([unistd.h stdint.h inttypes.h sys/types.h sys/wait.h])

dnl Checks for library functions.
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([open close getenv basename])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_STRUCT_TM
AC_STRUCT_TIMEZONE
AC_C_BIGENDIAN
AC_TYPE_INT32_T
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
KPSE_LIBPAPER_FLAGS
AC_DEFINE([HAVE_LIBPAPER], [1], [Define if you have libpaper.])

KPSE_SAVE_FLAGS

if test "x$enable_build" != xno || test -f config.force; then

KPSE_ADD_FLAGS([kpathsea])
AC_CHECK_HEADER([kpathsea/version.h], [],
                [AC_MSG_ERROR([Sorry, kpathsea too old])])

KPSE_ADD_FLAGS([zlib])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <zlib.h>]],
                                [[Bytef *dst,*src; uLong dstlen, srclen; int lvl, err;
                                  err=compress2(dst, &dstlen, src, srclen, lvl);]])],
               [],
               [AC_MSG_ERROR([Sorry, you need zlib with compress2])])

KPSE_ADD_FLAGS([libpng])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <png.h>]],
                                [[png_const_structp png; png_const_infop info; int err;
                                  err=png_get_image_width(png, info);]])],
               [],
               [AC_MSG_ERROR([Sorry, you need libpng])])

KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

dnl We require zlib with compress2 and libpng
AC_DEFINE([HAVE_ZLIB], 1, [Define if you have zlib and its headers.])
AC_DEFINE([HAVE_ZLIB_COMPRESS2], 1, [Define if your zlib has the compress2 function.])
AC_DEFINE([HAVE_LIBPNG], 1, [Define if you have libpng and its headers.])

KPSE_WIN32_CALL

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
