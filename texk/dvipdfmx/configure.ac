dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([dvipdfmx], [20090522], [dvipdfmx@project.ktug.or.kr])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([src/dvipdfmx.c])
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

KPSE_ADD_FLAGS([kpathsea])
CHO_CHECK_KPSE_SUPPORT([dvipdfmx])
CHO_CHECK_KPSE_TDS_VERSION

KPSE_ADD_FLAGS([zlib])
CHO_CHECK_ZLIB

KPSE_ADD_FLAGS([libpng])
CHO_CHECK_LIBPNG

KPSE_RESTORE_FLAGS

dnl We do not provide the configure option '--enable-tetex-build', but may
dnl inherit enable_tetex_build when part of a teTeX / TeX Live installation.
dnl If not, check for libpaper.
if test "x$enable_tetex_build" != xyes || test ! -d $srdir/../kpathsea; then
  CHO_CHECK_LIBPAPER
fi

AC_CONFIG_FILES([Makefile data/Makefile src/Makefile])

AC_OUTPUT
