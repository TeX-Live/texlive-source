dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2012 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define cairo_version
AC_INIT([cairo for TeX Live], cairo_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([cairo-]cairo_version[/src/cairo.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE
AC_PROG_CC

LT_INIT

KPSE_PIXMAN_FLAGS

AM_CONDITIONAL([build], [test "x$enable_build" != xno])
AC_SEARCH_LIBS([sqrt], [m])

if test "x$enable_build" != xno || test -f config.force; then

KPSE_ADD_FLAGS([pixman])
AC_CHECK_FUNC([pixman_version_string], , [AC_ERROR([pixman not found])])
KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

if test "x$host" = x; then
  config_args=
else
  config_args="--host=$host"
  if test "x$build" != x; then
    config_args="$config_args --build=$build"
  fi
fi
AC_SUBST([config_args])

AC_SUBST([CAIRO_TREE], [cairo-]cairo_version)

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
