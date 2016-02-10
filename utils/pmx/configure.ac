dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2012-2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define pmx_version
AC_INIT([pmx (TeX Live)], pmx_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([pmx-src/pmxab.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([pmx])

dnl Check compiler and flags
AC_PROG_CC

dnl initial value for PMX_DEFINES
AC_SUBST([PMX_DEFINES], ['-DNON_UNIX_STDIO'])

KPSE_COND_WIN32
dnl WIN32 has no <sys/times.h>, native WIN32 is as MSDOS
AS_CASE([$kpse_cv_have_win32],
        [native], [PMX_DEFINES="$PMX_DEFINES -DMSDOS"],
        [mingw32], [PMX_DEFINES="$PMX_DEFINES -DUSE_CLOCK"])

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AC_SEARCH_LIBS([sqrt], [m])

AC_CHECK_SIZEOF([long long])
if test $ac_cv_sizeof_long_long -lt 8; then
  PMX_DEFINES="$PMX_DEFINES -DNO_LONG_LONG"
fi

AC_SUBST([PMX_TREE], [pmx-src])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
