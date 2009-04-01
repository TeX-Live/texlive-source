dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([xdvik_version], [22.84.15])
AC_INIT([xdvik for TeX Live], xdvik_version, [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([xdvi.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([xdvik])

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

KPSE_KPATHSEA_FLAGS
KPSE_T1LIB_FLAGS

XDVIK_VERSION=xdvik_version
AC_SUBST([XDVIK_VERSION])

AC_CONFIG_SUBDIRS([squeeze])

AC_CONFIG_FILES([Makefile])

dnl The subdirectory squeeze must be configured for the build system.
dnl Can not share the cache file with the subdirectory!
AC_CONFIG_COMMANDS_POST([AS_IF([test "x$cross_compiling" = xyes],
[cache_file=/dev/null
 ac_configure_args="$ac_configure_args --host='$build' \
 CC='$BUILDCC' CPPFLAGS='$BUILDCPPFLAGS'\
 CFLAGS='$BUILDCFLAGS' LDFLAGS='$BUILDLDFLAGS'"])])

AC_OUTPUT
