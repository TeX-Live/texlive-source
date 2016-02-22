dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define zziplib_version
AC_INIT([TECkit (TeX Live)], TECkit_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([TECkit-src/source/UnicodeNames.cpp])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([tekcit])

# Checks for programs.
AC_PROG_CXX
AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

KPSE_CXX_HACK

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h])

AC_C_BIGENDIAN

AC_CONFIG_HEADERS([config.h])

KPSE_ZLIB_FLAGS

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

if test "x$enable_build" != xno || test -f config.force; then

KPSE_ADD_FLAGS([zlib])

AC_CHECK_FUNC([compress], , [AC_ERROR([zlib not found])])
AC_CHECK_HEADERS([zlib.h], , [AC_ERROR([zlib not found])])

KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

AC_DEFINE([HAVE_LIBZ], 1, [Define if you have zlib.])

AC_SUBST([TECKIT_TREE], [TECkit-src])

AC_CONFIG_FILES([Makefile include/teckit/Makefile])

AC_OUTPUT
