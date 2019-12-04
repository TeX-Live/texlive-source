dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2013, 2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define lz4_version
AC_INIT([lz4 (TeX Live)], lz4_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([lz4-src/lib/lz4.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([lz4])

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

AC_SUBST([LZ4VERSION], [lz4_version])
AC_SUBST([LZ4_TREE], [lz4-src])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
