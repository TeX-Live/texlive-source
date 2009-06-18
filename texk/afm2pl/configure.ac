dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([afm2pl for TeX Live], [0.7.03], [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([afm2pl.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([afm2pl])

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

AC_SEARCH_LIBS([floor], [m])

KPSE_KPATHSEA_FLAGS

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
