dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2016-2022 Karl Berry <tex-live@tug.org>
dnl   Copyright 2009-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([dvipsk (TeX Live)], [2022.1], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([dvips.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

# Common code for all programs using libkpathsea.
KPSE_COMMON([dvipsk])
KPSE_CANONICAL_HOST

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

AC_CHECK_SIZEOF([int])

AC_SEARCH_LIBS([pow], [m])

m4_include([../../version.ac])[] dnl define tex_live_version
AC_DEFINE([TL_VERSION], ["TeX Live tex_live_version()"],
          [Define to the current TeX Live version string.])

KPSE_KPATHSEA_FLAGS

# For dvips-all.enc.
encdir='${datarootdir}/texmf-dist/fonts/enc/dvips/base'
AC_SUBST([encdir])

AC_CONFIG_FILES([Makefile])

# The subdirectory squeeze must be configured for the build system.
KPSE_NATIVE_SUBDIRS([squeeze])

AC_OUTPUT
