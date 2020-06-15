dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([detex_version], [2.8.7])[]dnl using unmodified detex source tree
AC_INIT([detex (TeX Live)], detex_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([detex-src/detex.l])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([detex])

AC_PROG_LEX
AC_HEADER_TIME

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

KPSE_KPATHSEA_FLAGS

DETEX_TREE=detex-src
AC_SUBST([DETEX_TREE])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
