dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define xpdfopen_version
AC_INIT([xpdfopen (TeX Live)], xpdfopen_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([xpdfopen-src/pdfopen.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([xpdfopen])

AC_PROG_CC

dnl Find Xlibs.
AC_PATH_XTRA

AC_SUBST([XPDFOPEN_TREE], [xpdfopen-src])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
