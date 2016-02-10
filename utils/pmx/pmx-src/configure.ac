dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2012 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define pmx_version
AC_INIT([pmx], pmx_version, [rdt@cs.queensu.ca])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([pmxab.c])

AM_INIT_AUTOMAKE(pmx, pmx_version)

dnl Check compiler and flags
AC_PROG_CC

dnl initial value for AM_CPPFLAGS
AC_SUBST([AM_CPPFLAGS], ['-DNON_UNIX_STDIO'])

dnl WIN32 has no <sys/times.h>
AM_CONDITIONAL([WIN32], [test "x$kpse_cv_have_win32" != xno])
if test "x$kpse_cv_have_win32" != xno; then
  AM_CPPFLAGS="$AM_CPPPFLAGS -DUSE_CLOCK"
fi

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AC_SEARCH_LIBS([sqrt], [m])

AC_CHECK_SIZEOF([long long])
if test $ac_cv_sizeof_long_long -lt 8; then
  AM_CPPFLAGS="$AM_CPPPFLAGS -DNO_LONG_LONG"
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
