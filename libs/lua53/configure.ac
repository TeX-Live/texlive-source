dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2013, 2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define lua53_version
AC_INIT([lua53 (TeX Live)], lua53_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([lua53-src/src/lua.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([lua53])

AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_RANLIB
AC_PROG_LN_S

AC_SUBST([LUA53VERSION], [lua53_version])
KPSE_LT_VERSION([lua53])

LT_INIT([win32-dll])

KPSE_DLL_NAME([texlua], [texlua])

AC_CONFIG_HEADERS([config.h])[]dnl unused, to avoid zillions of -D's on command line

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

AC_SEARCH_LIBS([pow], [m])

KPSE_LUA53_DEFINES
KPSE_LARGEFILE([LUA53_DEFINES])

if test "x$ac_cv_header_stdc" = xyes; then
  LUA53_DEFINES="$LUA53_DEFINES -DHAVE_UNISTD_H"
fi

LUA53_DEFINES="$LUA53_DEFINES -DLUA_COMPAT_5_2"
LUA53_CFLAGS="-DLUA_COMPAT_5_2"

AC_SUBST([LUA53_TREE], [lua53-src])

AC_CONFIG_FILES([Makefile include/Makefile texlua53.pc])

AC_OUTPUT
