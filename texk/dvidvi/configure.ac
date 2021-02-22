dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([dvidvi (TeX Live)], [1.1], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([dvidvi.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([dvidvi])

AC_CONFIG_HEADERS([config.h])

m4_include([../../version.ac])[] dnl define tex_live_version
AC_DEFINE([TL_VERSION], ["TeX Live tex_live_version()"],
          [Define to the current TeX Live version string.])

# We don't use libpathsea, so add '-DNO_DEBUG' not to overload fopen.
KPSE_KPATHSEA_FLAGS([no-debug])
KPSE_COND_WIN32

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
