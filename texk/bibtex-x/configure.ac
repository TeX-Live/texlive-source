dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2015-2022 Karl Berry <tex-live@tug.org>
dnl   Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([extended BibTeX (TeX Live)], [4.01], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([bibtex-1.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([bibtex-x], [no-define])

m4_include([ac/bibtex-x.ac])

AC_CONFIG_HEADERS([config.h])

m4_include([../../version.ac])[] dnl define tex_live_version
AC_DEFINE([TL_VERSION], ["TeX Live tex_live_version()"],
          [Define to the current TeX Live version string.])

AC_PROG_CXX
KPSE_CXX_HACK

AC_SEARCH_LIBS([pow], [m])

if test "x$enable_bibtexu" != xno; then
  dnl As of ICU 65.1, ICU requires -lpthread on x86_64-linux and, most
  dnl likely, on other platforms. See upmendex/configure.ac for more info.
KPSE_KPATHSEA_FLAGS
KPSE_SEARCH_LIBS([PTHREAD_LIB], [pthread_create], [pthread])
KPSE_ICU_FLAGS([icuio icui18n], [--ldflags-icuio])
fi

AM_CONDITIONAL([BIBTEX8], [test "x$enable_bibtex8" != xno])
AM_CONDITIONAL([BIBTEXU], [test "x$enable_bibtexu" != xno])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
