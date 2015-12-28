dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define potrace_version
AC_INIT([potrace (TeX Live)], potrace_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([potrace-src/src/trace.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([potrace])

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S
AC_C_INLINE

AC_CONFIG_HEADERS([config.h])

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

AC_HEADER_STDC
if test "x$ac_cv_header_stdc" != xyes; then
  AC_MSG_ERROR([Sorry, can't do anything for you])
fi

AC_SEARCH_LIBS([sqrt], [m])

AC_SUBST([POTRACE_TREE], [potrace-src])

AC_CONFIG_FILES([Makefile include/Makefile])

AC_OUTPUT
