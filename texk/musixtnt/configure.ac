dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define musixtnt_version
AC_INIT([musixtnt (TeX Live)], musixtnt_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([musixtnt-src/msxlint.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([musixtnt])

AC_HEADER_STDBOOL

AC_CONFIG_HEADERS([config.h])

KPSE_KPATHSEA_FLAGS

AC_SUBST([MUSIXTNT_TREE], [musixtnt-src])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
