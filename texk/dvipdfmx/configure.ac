dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([dvipdfmx], [20080607], [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([src/dvipdfmx.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([dvipdfmx])

AC_CONFIG_HEADERS([config.h])

# TEXMF='${datadir}/texmf'
# AC_SUBST(TEXMF)

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdint.h inttypes.h sys/types.h])

dnl Checks for library functions.
AC_FUNC_MEMCMP
AC_CHECK_FUNCS([open close system getenv mkstemp basename])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_STRUCT_TM
AC_STRUCT_TIMEZONE
AC_C_BIGENDIAN

MAW_EXT_TIMEZONE
MAW_TM_HAS_TM_GMTOFF

AC_CHECK_SIZEOF([char])
AC_CHECK_SIZEOF([short])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])

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
                                [[kpse_tex_ps_header_format;
                                  kpse_type1_format;
                                  kpse_vf_format;
                                  kpse_ofm_format;
                                  kpse_ovf_format;
                                  kpse_truetype_format;]])],
               [AC_MSG_RESULT([yes])
                AC_MSG_CHECKING([whether your kpathsea has xbasename])
                AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>
#include <kpathsea/lib.h>]],
                                                [[xbasename;]])],
                               [AC_MSG_RESULT([yes])],
                               [AC_MSG_RESULT([no])
                                AC_MSG_ERROR([This version of ]Kpse_Package[ requires xbasename() in kpathsea library.])])],
               [AC_MSG_RESULT([no])
                AC_MSG_ERROR([Required file formats not found in Kpathsea header files.

This version of ]Kpse_Package[ requires that kpathsea and its headers be available.
If you are sure they are installed and in a standard place, maybe you need a
newer version of kpathsea?  You also might try setting the environment
variable CPPFLAGS (or CFLAGS) with -I pointing to the directory containing
the file "kpathsea/tex-file.h"

])])

AC_MSG_CHECKING([whether kpathsea library supports TDS version 1.1 installation])
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>
#include <kpathsea/kpathsea.h>
#include <kpathsea/tex-file.h>]],
                                [[kpse_enc_format;
                                  kpse_cmap_format;
                                  kpse_sfd_format;
                                  kpse_opentype_format;]])],
               [tds_version_11=yes
                AC_DEFINE([__TDS_VERSION__], [0x200406L],
                          [Define as 0x200406L if your libkpathsea supports enc formats,
                           or as 0x200302L otherwise.])],
               [tds_version_11=no
                AC_DEFINE([__TDS_VERSION__], [0x200302L])])
AC_MSG_RESULT([$tds_version_11])
AM_CONDITIONAL([TDS_VERSION_11], [test "x$tds_version_11" = xyes])

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

dnl We do not provide the configure option '--enable-tetex-build, but may
dnl inherit enable_tetex_build when part of a teTeX / TeX Live installation.
dnl If not, check for libpaper.
if test "x$enable_tetex_build" != xyes || test ! -d $srdir/../kpathsea; then
  CHO_HAS_LIBPAPER
fi

AC_CONFIG_FILES([Makefile data/Makefile src/Makefile])

AC_OUTPUT
