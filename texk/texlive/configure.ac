dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([../../version.ac])[] dnl define tex_live_version
AC_INIT([TeX Live Scripts], tex_live_version(), [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([linked_scripts/ChangeLog])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

# Configure options for texlive also shown at the TeX Live top-level.
m4_include([ac/texlive.ac])
AM_CONDITIONAL([LINKED_SCRIPTS], [test "x$enable_linked_scripts" != xno])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET

KPSE_COND_WIN32_WRAP

AC_CONFIG_FILES([Makefile
  linked_scripts/Makefile
  tl_scripts/Makefile])

AC_OUTPUT
