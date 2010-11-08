dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([dialog_version], [1.1-20100428])[]dnl using unmodified dialog source tree
AC_PREREQ([2.65])
AC_INIT([dialog for TeX Live], dialog_version, [peb@mppmu.mpg.de])
AC_CONFIG_SRCDIR([dialog-]dialog_version[/dialog.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

# Checks for programs.
AC_PROG_CC
AC_PROG_MAKE_SET
AC_PROG_INSTALL

DIALOG_TREE=[dialog-]dialog_version
AC_SUBST([DIALOG_TREE])

# Initialize the subdir machinery
AC_CONFIG_SUBDIRS([])

if test "x$enable_build" != xno || test -f config.force; then
  # when building dialog, configure the subdir $DIALOG_TREE
  # but prevent autoreconf from recursing into this subdir.
  subdirs=$DIALOG_TREE
  echo timestamp >config.force
fi

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
