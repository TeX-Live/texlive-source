dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([icu_version], [4.2.1])[]dnl using modified icu/source source tree
AC_INIT([icu for TeX Live], icu_version, [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([icu-]icu_version[/common/unicode/utypes.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE
KPSE_CANONICAL_HOST

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

AM_CONDITIONAL([cross], [test "x$cross_compiling" = xyes])

AC_PROG_LN_S

test "x$srcdir" = x. && srcdir=../icu

ICU_TREE=[icu-]icu_version
AC_SUBST([ICU_TREE])

AC_CONFIG_SUBDIRS([dummy])

AC_CONFIG_FILES([Makefile icu-incl/Makefile icu-incl/unicode/Makefile icu-incl/layout/Makefile])

AC_OUTPUT
