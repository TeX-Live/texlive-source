dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2018 Karl Berry <tex-live@tug.org>
dnl   Copyright 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define freetype_version
AC_INIT([FreeType (TeX Live)], freetype_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([freetype-src/builds/unix/ftconfig.in])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([freetype2])

AC_PROG_CC

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

# pass explicit --host and --build, but nothing else?
if test "x$host" = x; then
  config_args=
else
  config_args="--host=$host"
fi
if test "x$build" != x; then
  config_args="$config_args --build=$build"
fi

AC_SUBST([config_args])

AC_SUBST([FREETYPE_TREE], [freetype-src])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
