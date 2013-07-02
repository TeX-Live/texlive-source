dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([dviljk (TeX Live)], [2.6p5], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([dvi2xx.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([dviljk], [no-define])

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

AC_C_CHAR_UNSIGNED
AC_CHECK_SIZEOF([long])
AC_CHECK_FUNCS([rmdir unlink mkdtemp mktemp])

KPSE_KPATHSEA_FLAGS

KPSE_WIN32_CALL

AC_CONFIG_FILES([Makefile win32/Makefile])

AC_OUTPUT
