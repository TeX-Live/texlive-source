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

KPSE_ADD_FLAGS([kpathsea])

dnl Check for required file formats.
dnl We need tex-ps_header, type1, and vf;
dnl of these type1 was introduced last (Feb 1996).
KPSE_CHECK_KPSE_FORMAT([type1], ,
                       [KPSE_MSG_ERROR([dvipdfm],
                                       [Required file formats not found in Kpathsea header files.

This version of dvipdfm requires that kpathsea and its headers be available.
If you are sure they are installed and in a standard place, maybe you need a
newer version of kpathsea?  You also might try setting the environment
variable CPPFLAGS (or CFLAGS) with -I pointing to the directory containing
the file "kpathsea/kpathsea.h"

])])

dnl Check for ofm format.
KPSE_CHECK_KPSE_FORMAT([ofm],
                       [AC_DEFINE([HAVE_OMEGA_FORMATS], 1,
                                  [Define if kpathsea understands Omega (OFM/OVF) formats.])])

dnl Check for truetype format.
KPSE_CHECK_KPSE_FORMAT([truetype],
                       [AC_DEFINE([HAVE_TTF_FORMATS], 1,
                                  [Define if kpathsea understands TrueType (TTF) formats.])])
AM_CONDITIONAL([HAVE_TTF], [test "x$kpse_cv_have_truetype_format" = xyes])

dnl Check for xbasename.
KPSE_CHECK_XBASENAME([AC_DEFINE([HAVE_XBASENAME], 1,
                                [Define if you have xbasename() in your libkpathsea
                                 instead of basename().])])

KPSE_ADD_FLAGS([zlib])
CHO_CHECK_ZLIB

KPSE_ADD_FLAGS([libpng])
CHO_CHECK_LIBPNG

KPSE_RESTORE_FLAGS

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
