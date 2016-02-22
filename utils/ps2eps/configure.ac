dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([ps2eps_version], [1.68])[]dnl using unmodified ps2eps source tree
AC_INIT([ps2eps (TeX Live)], ps2eps_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([ps2eps-src/bin/ps2eps.pl])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([ps2eps])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

KPSE_COND_WIN32_WRAP

AC_SUBST([PS2EPS_TREE], [ps2eps-src])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
