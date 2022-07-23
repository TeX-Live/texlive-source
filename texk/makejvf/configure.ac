dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2010-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([makejvf (TeX Live)], [20220723])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([makejvfk], [no-define])

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

AC_CHECK_HEADERS([strings.h])

KPSE_KPATHSEA_FLAGS
KPSE_PTEXENC_FLAGS

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
