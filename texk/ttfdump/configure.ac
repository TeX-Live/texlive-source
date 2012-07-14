dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2012 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([ttfdump for TeX Live], [0.5.5], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([include/ttf.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([ttfdump])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_CHECK_SIZEOF([int])

AC_CONFIG_HEADERS([config.h])

KPSE_KPATHSEA_FLAGS

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
