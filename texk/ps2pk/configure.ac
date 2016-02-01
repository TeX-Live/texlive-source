dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2009-2016 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([ps2pkm (TeX Live)], [1.8], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([arith.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([ps2pkm], [no-define])

AC_SEARCH_LIBS([pow], [m])

dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([fcntl.h])

dnl Checks for types.
AC_TYPE_INT32_T
AC_TYPE_INTPTR_T
AC_TYPE_UINT32_T

dnl Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([memset])

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

dnl For tests
AC_CHECK_PROGS([KPSEWHICH], [kpsewhich], [false])

KPSE_KPATHSEA_FLAGS

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
