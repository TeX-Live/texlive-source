dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2012 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define pmx_version
AC_INIT([pmx for TeX Live], pmx_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([pmx-]pmx_version[/scor2prt.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

dnl Check compiler and flags
AC_PROG_CC

KPSE_COMPILER_WARNINGS

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

AC_SEARCH_LIBS([sqrt], [m])

AC_CHECK_SIZEOF([long long])
if test $ac_cv_sizeof_long_long -lt 8; then
  ARITHCHK_CPPFLAGS='-DNO_LONG_LONG -DNO_FPINIT'
else
  ARITHCHK_CPPFLAGS='-DNO_FPINIT'
fi
AC_SUBST([ARITHCHK_CPPFLAGS])

AC_CONFIG_LINKS([pmxab.c:pmx-]pmx_version[/pmx]m4_bpatsubsts(pmx_version, [\.], [])[.c])

AC_SUBST([PMX_TREE], [pmx-]pmx_version)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
