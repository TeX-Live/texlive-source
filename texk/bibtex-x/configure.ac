dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([extended BibTeX (TeX Live)], [3.71], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([bibtex-1.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([bibtex-x], [no-define])

m4_include([ac/bibtex-x.ac])

AC_CONFIG_HEADERS([config.h])

AC_PROG_CXX
KPSE_CXX_HACK

AC_SEARCH_LIBS([pow], [m])

KPSE_KPATHSEA_FLAGS
KPSE_ICU_FLAGS([icuio icui18n], [--ldflags-icuio])

AM_CONDITIONAL([BIBTEX8], [test "x$enable_bibtex8" != xno])
AM_CONDITIONAL([BIBTEXU], [test "x$enable_bibtexu" != xno])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
