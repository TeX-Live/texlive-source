dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([dialog_version], [1.1-20080819])[]dnl using unmodified dialog-1.1-20080819 source tree
AC_PREREQ([2.63])
AC_INIT([dialog for TeX Live], dialog_version, [peb@mppmu.mpg.de])
AC_CONFIG_SRCDIR([dialog-]dialog_version[/dialog.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign dist-bzip2])
AM_MAINTAINER_MODE

DIALOG_TREE=dialog-[]dialog_version
AC_SUBST([DIALOG_TREE])

if test "x$srcdir" = x.; then
  DIALOG_BUILD=$DIALOG_TREE
else
  DIALOG_BUILD=dialog-build
fi
AC_SUBST([DIALOG_BUILD])

# Initialize the subdir machinery
AC_CONFIG_SUBDIRS([])

# when building dialog, configure the original source tree,
# prevent autoreconf from recursing into subdir.
test "x$enable_dialog" = xyes && subdirs=$DIALOG_TREE
AM_CONDITIONAL([build], [test "x$enable_dialog" = xyes])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT

# Rename the subdirectory $DIALOG_TREE just crated into $DIALOG_BUILD
if test -d "$DIALOG_TREE" && test "x$DIALOG_BUILD" != "x$DIALOG_TREE"; then
  rm -rf "$DIALOG_BUILD" && mv "$DIALOG_TREE" "$DIALOG_BUILD" || exit 1
fi
