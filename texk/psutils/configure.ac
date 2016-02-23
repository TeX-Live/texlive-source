dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define psutils_version
AC_INIT([psutils (TeX Live)], psutils_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([psutils-src/psutil.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_COMMON([psutils])

AC_CONFIG_HEADERS([config.h])

AH_TOP([#ifndef _PSUTILS_CONFIG_H
#define _PSUTILS_CONFIG_H 1])
AH_BOTTOM([#include <texlive.h>

#endif /* _PSUTILS_CONFIG_H */])

KPSE_KPATHSEA_FLAGS
KPSE_LIBPAPER_FLAGS

KPSE_COND_WIN32_WRAP

AC_SUBST([PSUTILS_TREE], [psutils-src])

AC_CONFIG_FILES([Makefile])

AC_CONFIG_LINKS([extractres.pl:psutils-src/extractres])
AC_CONFIG_LINKS([includeres.pl:psutils-src/extractres])
AC_CONFIG_LINKS([psjoin.pl:psutils-src/psjoin])

AC_OUTPUT
