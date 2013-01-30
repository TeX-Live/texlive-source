dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([psutils_version], [1.17])[]dnl using unmodified psutils source tree
AC_INIT([psutils for TeX Live], psutils_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([psutils-]psutils_version[/psutil.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([psutils])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

KPSE_COND_WIN32_WRAP

PSUTILS_TREE=psutils-[]psutils_version
AC_SUBST([PSUTILS_TREE])

AC_CONFIG_FILES([Makefile
                 patchlev.h:psutils-]psutils_version[/patchlev.h.in])

AC_OUTPUT
