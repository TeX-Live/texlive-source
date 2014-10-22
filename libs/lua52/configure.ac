dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2013, 2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define lua52_version
AC_INIT([lua52 (TeX Live)], lua52_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([lua-]lua52_version[/src/lua.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([lua52])

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

AC_SUBST([LUA52VERSION], [lua52_version])
KPSE_LT_VERSION([lua52])

LT_INIT([win32-dll])

KPSE_DLL_NAME([texlua52], [texlua52])

AC_CONFIG_HEADERS([config.h])[]dnl unused, to avoid zillions of -D's on command line

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

AC_SEARCH_LIBS([sqrt], [m])

KPSE_LUA52_DEFINES
KPSE_LARGEFILE([LUA52_DEFINES])

if test "x$ac_cv_header_stdc" = xyes; then
  LUA52_DEFINES="$LUA52_DEFINES -DHAVE_UNISTD_H"
fi

AC_SUBST([LUA52_TREE], [lua-]lua52_version)

AC_CONFIG_FILES([Makefile include/Makefile texlua52.pc])

AC_OUTPUT
