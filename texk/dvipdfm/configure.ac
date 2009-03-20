dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([dvipdfm], [0.13.3TL], [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([dvipdfm.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([dvipdfm])

AC_CONFIG_HEADERS([config.h])

# TEXMF='${datadir}/texmf'
# AC_SUBST(TEXMF)

dnl Checks for header files.
AC_HEADER_STDC

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_STRUCT_TM

MAW_EXT_TIMEZONE
MAW_TM_HAS_TM_GMTOFF

dnl Checks for library functions.
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([strspn system])

AC_SEARCH_LIBS([pow], [m])

KPSE_KPATHSEA_FLAGS
KPSE_ZLIB_FLAGS
KPSE_LIBPNG_FLAGS

save_CPPFLAGS=$CPPFLAGS
save_LIBS=$LIBS

CPPFLAGS="$CPPFLAGS $KPATHSEA_INCLUDES"
LIBS="$KPATHSEA_LIBS $LIBS"

AC_MSG_CHECKING([whether you have kpathsea headers and they know about the required file formats])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>
#include <kpathsea/tex-file.h>]],
                                [[kpse_tex_ps_header_format; kpse_type1_format; kpse_vf_format;]])],
               [AC_MSG_RESULT([yes])
                AC_DEFINE([HAVE_KPATHSEA], 1,
                          [Define if you have kpathsea.])
                AC_DEFINE([HAVE_KPSE_FORMATS], 1,
                          [Define if kpathsea supports type1 and vf formats.])],
               [AC_MSG_RESULT([no])
                AC_MSG_ERROR([PS_HEADER and/or VF formats not found in Kpathsea header files.

This version of dvipdfm requires that kpathsea and its headers be available.
If you are sure they are installed and in a standard place, maybe you need a
newer version of kpathsea?  You also might try setting the environment
variable CPPFLAGS (or CFLAGS) with -I pointing to the directory containing
the file "kpathsea/tex-file.h"

])])

AC_MSG_CHECKING([whether your kpathsea supports Omega OFM file formats])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>
#include <kpathsea/tex-file.h>]],
                                [[kpse_ofm_format;]])],
               [AC_MSG_RESULT([yes])
                AC_DEFINE([HAVE_OMEGA_FORMATS], 1,
                          [Define if kpathsea understands Omega (OFM/OVF) formats.])],
               [AC_MSG_RESULT([no])])

AC_MSG_CHECKING([whether your kpathsea supports Truetype (TTF) file formats])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>
#include <kpathsea/tex-file.h>]],
                                [[kpse_truetype_format;]])],
               [have_ttf=yes
                AC_DEFINE([HAVE_TTF_FORMATS], 1,
                          [Define if kpathsea understands TrueType (TTF) formats.])],
               [have_ttf=no])
AC_MSG_RESULT([$have_ttf])
AM_CONDITIONAL([HAVE_TTF], [test "x$have_ttf" = xyes])

AC_MSG_CHECKING([whether your kpathsea has xbasename])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>
#include <kpathsea/lib.h>]],
                                [[xbasename;]])],
               [AC_MSG_RESULT([yes])
                AC_DEFINE([HAVE_XBASENAME], 1,
                          [Define if you have xbasename() in your libkpathsea instead of basename().])],
               [AC_MSG_RESULT([no])])

CPPFLAGS="$save_CPPFLAGS $ZLIB_INCLUDES"
LIBS="$ZLIB_LIBS $save_LIBS"

AC_MSG_CHECKING([for zlib header files and library])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <zlib.h>]],
                                [[z_stream p;]])],
               [AC_MSG_RESULT([yes])
                AC_CHECK_FUNCS([compress],
                               [AC_DEFINE([HAVE_ZLIB], 1,
                                          [Define if you have zlib and its headers.])
                                AC_CHECK_FUNCS([compress2],
                                               [AC_DEFINE([HAVE_ZLIB_COMPRESS2], 1,
                                                          [Define if your zlib has the compress2 function.])])])],
               [AC_MSG_RESULT([no])])

CPPFLAGS="$CPPFLAGS $LIBPNG_INCLUDES"
LIBS="$LIBPNG_LIBS $LIBS"

AC_MSG_CHECKING([for png header files and library])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <png.h>]],
                                [[png_infop p;]])],
               [AC_MSG_RESULT([yes])
                AC_CHECK_FUNCS([png_get_image_width],
                               [AC_DEFINE([HAVE_LIBPNG], 1,
                                          [Define if you have libpng and its headers.])])],
               [AC_MSG_RESULT([no])])

CPPFLAGS=$save_CPPFLAGS
LIBS=$save_LIBS

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
