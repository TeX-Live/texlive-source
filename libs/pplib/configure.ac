dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2013, 2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define pplib_version
AC_INIT([pplib (TeX Live)], pplib_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([pplib-src/src/pplib.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([pplib])

AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_RANLIB
AC_PROG_LN_S

AC_SUBST([PPLIBVERSION], [pplib_version])
KPSE_LT_VERSION([pplib])

LT_INIT([win32-dll])

KPSE_DLL_NAME([pplib], [pplib])

AC_CONFIG_HEADERS([config.h])[]dnl unused, to avoid zillions of -D's on command line

AM_CONDITIONAL([build], [test "x$enable_build" != xno])


#KPSE_PPLIB_DEFINES
KPSE_LARGEFILE([PPLIB])

KPSE_ZLIB_FLAGS

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

if test "x$enable_build" != xno || test -f config.force; then

KPSE_ADD_FLAGS([zlib])
AC_CHECK_FUNC([zlibVersion], , [AC_ERROR([zlib not found])])
KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi



if test "x$ac_cv_header_stdc" = xyes; then
  PPLIB_DEFINES="$PPLIB_DEFINES -DHAVE_UNISTD_H"
fi

#PPLIB_DEFINES="$PPLIB_DEFINES -DEXTRA"
#PPLIB_CFLAGS="-DEXTRA"

AC_SUBST([PPLIB_TREE], [pplib-src])

AC_CONFIG_FILES([Makefile include/Makefile])

AC_OUTPUT
