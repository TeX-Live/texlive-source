# cho-libs.m4 - various libraries
# Copyright (C) 1998 - 2008 Jin-Hwan Cho <chofchof@ktug.or.kr>
#
# This file is free software; the copyright holders
# give unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 1

# CHO_HAS_ZLIB
# ------------
# Check for zlib
AC_DEFUN([CHO_HAS_ZLIB],
[_cppflags=$CPPFLAGS _ldflags=$LDFLAGS
 AC_ARG_WITH([zlib],  
  [AS_HELP_STRING([--with-zlib=DIR], [use zlib include/library files from DIR])],
  [if test -d "$withval"; then
     CPPFLAGS="$CPPFLAGS -I$withval/include"
     LDFLAGS="$LDFLAGS -L$withval/lib"
   fi])
 AC_MSG_CHECKING([for zlib header files])
 AC_TRY_COMPILE(
  [#include <zlib.h>],
  [z_stream p;],
  [AC_MSG_RESULT(yes)
      AC_SEARCH_LIBS([compress], [z],
       [AC_DEFINE([HAVE_ZLIB], 1, [Define if you have zlib and its headers])
        AC_CHECK_LIB([z], [compress2],
         [AC_DEFINE([HAVE_ZLIB_COMPRESS2], 1, [Define if your zlib has the compress2 function])])])],
  [CPPFLAGS=$_cppflags
   LDDFLAGS=$_ldflags
   AC_MSG_RESULT(no)])
])# CHO_HAS_ZLIB

# CHO_HAS_LIBPNG
# --------------
# Check for libpng
AC_DEFUN([CHO_HAS_LIBPNG],
[_cppflags=$CPPFLAGS _ldflags=$LDFLAGS
 AC_ARG_WITH([png],
  [AS_HELP_STRING([--with-png=DIR], [use png include/library files from DIR])],
  [if test -d "$withval"; then
     CPPFLAGS="$CPPFLAGS -I$withval/include"
     LDFLAGS="$LDFLAGS -L$withval/lib"
   fi])
 AC_SEARCH_LIBS([pow], [m])
 AC_MSG_CHECKING([for png header files])
 AC_TRY_COMPILE(
  [#include <png.h>],
  [png_infop p;],
  [AC_MSG_RESULT(yes)
      AC_SEARCH_LIBS(png_get_image_width, png, 
       [AC_DEFINE([HAVE_LIBPNG], 1, [Define if you have libpng])],
       [], [-lz])],
  [CPPFLAGS=$_cppflags
   LDDFLAGS=$_ldflags
   AC_MSG_RESULT(no)])
])# CHO_HAS_LIBPNG

# CHO_HAS_LIBPAPER
# ----------------
# Check for libpaper
AC_DEFUN([CHO_HAS_LIBPAPER],
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
])# CHO_HAS_LIBPAPER

# CHO_HAS_LIBFONTCONFIG
# ---------------------
# Check for libfontconfig
AC_DEFUN([CHO_HAS_LIBFONTCONFIG],
[_cppflags=$CPPFLAGS _ldflags=$LDFLAGS
 have_libfontconfig=no
 AC_ARG_WITH([fontconfig],
  [AS_HELP_STRING([--with-fontconfig=DIR],[use fontconfig include/library files from DIR])],
  [if test -d "$withval"; then
     CPPFLAGS="$CPPFLAGS -I$withval/include"
     LDFLAGS="$LDFLAGS -L$withval/lib"
     FONTCONFIGLDFLAGS="-L$withval/lib"
   fi])
 AC_MSG_CHECKING([for fontconfig header files])
 AC_TRY_COMPILE(
  [#include <fontconfig/fontconfig.h>],
  [FcObjectSet *os;],
  [AC_MSG_RESULT(yes)
   AC_SEARCH_LIBS([FcInit], [fontconfig],
    [AC_DEFINE([HAVE_LIBFONTCONFIG], 1, [Define if you have libfontconfig])
     have_libfontconfig=yes])],
  [CPPFLAGS=$_cppflags
   LDDFLAGS=$_ldflags
   AC_MSG_RESULT(no)])
])# CHO_HAS_LIBFONTCONFIG

# CHO_CHECK_LIBFREETYPE(PACKAGE-NAME)
# -----------------------------------
# Check for libfreetype, fail with error if not found
AC_DEFUN([CHO_CHECK_LIBFREETYPE],
[_cppflags=$CPPFLAGS _ldflags=$LDFLAGS
 using_installed_freetype=yes
 AC_ARG_WITH(ft2lib,
  [AS_HELP_STRING([--with-ft2lib=LIB], [use freetype2 library LIB])],
  [if test x"$withval" != x; then
     LIBS="$LIBS $withval"
     using_installed_freetype=no
   fi])
 AC_ARG_WITH([ft2include],
  [AS_HELP_STRING([--with-ft2include=DIR], [use freetype2 include files from DIR])],
  [if test -d "$withval"; then
     CPPFLAGS="$CPPFLAGS -I$withval"
   fi])
 AC_ARG_WITH([freetype2],
  [AS_HELP_STRING([--with-freetype2=DIR], [use installed freetype2 include & lib files from DIR])],
  [if test -d "$withval"; then
     CPPFLAGS="$CPPFLAGS -I$withval/include/freetype2"
     LDFLAGS="$LDFLAGS -L$withval/lib"
   fi])
 AC_MSG_CHECKING([for freetype2 header files and library])
 AC_TRY_COMPILE(
  [#include "ft2build.h"
#include FT_FREETYPE_H],
  [FT_Face face;],
  [AC_MSG_RESULT(yes)
   if test x"$using_installed_freetype" = xyes; then
     AC_SEARCH_LIBS([FT_Init_FreeType], [freetype],
      [AC_DEFINE([HAVE_LIBFREETYPE], 1, [Define if you have libfreetype])])
   else
     AC_DEFINE([HAVE_LIBFREETYPE], 1)
   fi],
  [AC_MSG_RESULT(no)
   AC_MSG_ERROR([This version of $1 requires that FreeType2 and its headers be available.
You can use the --with-freetype2 option to indicate the location of the installed
freetype2, or --with-ft2lib and --with-ft2include to point to the libfreetype.a
library and the FreeType2 include files.])])
])# CHO_CHECK_LIBFREETYPE

