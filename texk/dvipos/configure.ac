dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([dvipos (TeX Live)], [20070107], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([dvipos.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([dvipos])

AC_CONFIG_HEADERS([config.h])[]dnl unused, to avoid zillions of -D's on command line

AC_SEARCH_LIBS([floor], [m])

KPSE_KPATHSEA_FLAGS

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
