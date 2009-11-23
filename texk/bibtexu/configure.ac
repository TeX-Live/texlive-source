dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([bibtexu for TeX Live], [3.71], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([bibtex-1.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([bibtexu], [no-define])

AC_CONFIG_HEADERS([config.h])

AC_PROG_CXX
KPSE_CXX_HACK

AC_SEARCH_LIBS([pow], [m])

KPSE_KPATHSEA_FLAGS
KPSE_ICU_FLAGS([icuio icui18n], [--ldflags-icuio])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
