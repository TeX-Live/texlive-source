dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([makeindex for TeX Live], [2.15], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([mkind.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([makeindexk], [no-define])

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

m4_include([../../version.ac])[] dnl define tex_live_version
AC_DEFINE([TEX_LIVE_VERSION], ["tex_live_version()"],
          [Define to the current TeX Live version string.])

AC_CHECK_HEADERS([ctype.h types.h locale.h])
AC_CHECK_FUNCS([strchr strrchr setlocale])

KPSE_KPATHSEA_FLAGS

KPSE_CHECK_WIN32
AM_CONDITIONAL([WIN32], [test "x$kpse_cv_have_win32" != xno])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
