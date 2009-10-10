dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([icu-xetex for TeX Live], [3.8.1], [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([icu-xetex/common/unicode/utypes.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign dist-bzip2])
AM_MAINTAINER_MODE

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

test "x$srcdir" = x. && srcdir=../icu

AC_CONFIG_SUBDIRS([dummy])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
