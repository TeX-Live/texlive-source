# Public macros for the TeX Live (TL) tree.
# Copyright (C) 1998 - 2008 Jin-Hwan Cho <chofchof@ktug.or.kr>
# Copyright (C) 2008 - 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 2

# CHO_CHECK_ZLIB
# --------------
# Check for zlib
AC_DEFUN([CHO_CHECK_ZLIB],
[AC_CACHE_CHECK([for zlib header files and library],
                [cho_cv_have_zlib],
                [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <zlib.h>]],
                                                 [[z_stream p;]])],
                                [cho_cv_have_zlib=yes],
                                [cho_cv_have_zlib=no])])
AS_IF([test "x$cho_cv_have_zlib" = xyes],
      [AC_CHECK_FUNCS([compress],
                      [AC_DEFINE([HAVE_ZLIB], 1,
                                 [Define if you have zlib and its headers.])
                       AC_CHECK_FUNCS([compress2],
                                      [AC_DEFINE([HAVE_ZLIB_COMPRESS2], 1,
                                                 [Define if your zlib has the compress2 function.])])])])
])# CHO_CHECK_ZLIB

# CHO_CHECK_LIBPNG
# ----------------
# Check for libpng
AC_DEFUN([CHO_CHECK_LIBPNG],
[AC_CACHE_CHECK([for libpng header files and library],
                [cho_cv_have_png],
                [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <png.h>]],
                                                 [[png_infop p;]])],
                                [cho_cv_have_png=yes],
                                [cho_cv_have_png=no])])
AS_IF([test "x$cho_cv_have_png" = xyes],
      [AC_CHECK_FUNCS([png_get_image_width],
                      [AC_DEFINE([HAVE_LIBPNG], 1,
                                 [Define if you have libpng and its headers.])])])
])# CHO_CHECK_LIBPNG

# CHO_CHECK_LIBPAPER
# ------------------
# Check for libpaper
AC_DEFUN([CHO_CHECK_LIBPAPER],
[_cppflags=$CPPFLAGS _ldflags=$LDFLAGS
 AC_ARG_WITH([paper],
  [AS_HELP_STRING([--with-paper=DIR], [use paper include/library files from DIR])],
  [if test -d "$withval"; then
     CPPFLAGS="$CPPFLAGS -I$withval/include"
     LDFLAGS="$LDFLAGS -L$withval/lib"
   fi])
 AC_MSG_CHECKING([for paper header files])
 AC_TRY_COMPILE(
  [#include <paper.h>],
  [struct paper *p;],
  [AC_MSG_RESULT(yes)
   AC_SEARCH_LIBS([paperpswidth], [paper],
    [AC_DEFINE(HAVE_LIBPAPER, 1, [Define if you have libpaper])])],
  [CPPFLAGS=$_cppflags
   LDDFLAGS=$_ldflags
   AC_MSG_RESULT(no)])
])# CHO_CHECK_LIBPAPER

# CHO_CHECK_LIBFONTCONFIG
# -----------------------
# Check for libfontconfig
AC_DEFUN([CHO_CHECK_LIBFONTCONFIG],
[AC_CACHE_CHECK([for fontconfig header files and library],
                [cho_cv_have_fontconfig],
                [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <fontconfig/fontconfig.h>]],
                                                 [[FcObjectSet *os;]])],
                                [cho_cv_have_fontconfig=yes],
                                [cho_cv_have_fontconfig=no])])
AS_IF([test "x$cho_cv_have_fontconfig" = xyes],
      [AC_CHECK_FUNCS([FcInit],
                      [AC_DEFINE([HAVE_LIBFONTCONFIG], 1,
                                 [Define if you have libfontconfig and its headers.])])])
])# CHO_CHECK_LIBFONTCONFIG

# CHO_CHECK_LIBFREETYPE(PACKAGE-NAME)
# -----------------------------------
# Check for libfreetype, fail with error if not found
AC_DEFUN([CHO_CHECK_LIBFREETYPE],
[AC_CACHE_CHECK([for freetype2 header files and library],
                [cho_cv_have_freetype],
                [AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <ft2build.h>
#include FT_FREETYPE_H]],
                                                 [[FT_Face face;]])],
                [cho_cv_have_freetype=yes],
                [cho_cv_have_freetype=no])])
AS_IF([test "x$cho_cv_have_freetype" = xyes],
      [AC_CHECK_FUNCS([FT_Init_FreeType],
                      [AC_DEFINE([HAVE_LIBFREETYPE], 1,
                                 [Define if you have libfreetype and its headers.])])],
      [AC_MSG_ERROR([This version of $1 requires that FreeType2 and its headers be available.
You can use the --with-freetype2-includes and/or --with-freetype2-libdir options
to indicate the location of the installed freetype2 library and headers.])])
])# CHO_CHECK_LIBFREETYPE

