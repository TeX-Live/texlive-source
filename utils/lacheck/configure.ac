dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2015-2019 Karl Berry <tex-live@tug.org>
dnl   Copyright 2009-2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([lacheck_version], [1.29])dnl duplicated in lacheck.l, sorry
AC_INIT([LaCheck (TeX Live)], lacheck_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([lacheck.l])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([lacheck])

AC_PROG_LEX

AC_CONFIG_HEADERS([config.h])[]dnl unused, to avoid zillions of -D's on command line

AC_SUBST([REV], [lacheck_version])

AC_CONFIG_FILES([Makefile lacheck.1:lacheck.man])

AC_OUTPUT
