dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([freetype_version], [1.5])[]dnl using unmodified?? freetype-1.5 source tree
AC_INIT([FreeType1 for TeX Live], freetype_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([freetype-]freetype_version[/lib/freetype.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE
AC_PROG_CC

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

if test "x$host" = x; then
  config_args=
else
  config_args="--host=$host"
  if test "x$build" != x; then
    config_args="$config_args --build=$build"
  fi
fi
AC_SUBST([config_args])

FREETYPE_TREE=[freetype-]freetype_version
AC_SUBST([FREETYPE_TREE])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
