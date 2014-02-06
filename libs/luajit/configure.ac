dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define luajit_version
AC_INIT([luajit for TeX Live], luajit_version, [luigi.scarso@gmail.com])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([LuaJIT-]luajit_version[/src/luajit.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([luajit])

AC_CANONICAL_HOST

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

AC_SEARCH_LIBS([sqrt], [m])

KPSE_COND_WIN32

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

AC_SUBST([LUAJIT_TREE], [LuaJIT-]luajit_version)

AC_CONFIG_FILES([Makefile])

AC_CONFIG_SUBDIRS([native])

AS_IF([test "x$cross_compiling" = xyes],
      [AC_SUBST([CROSS], [$host-])])

dnl The subdirectory native must be configured for the build system.
dnl When cross compiling, can not share the cache file with the subdirectory!
AC_CONFIG_COMMANDS_POST([AS_IF([test "x$cross_compiling" = xyes],
[cache_file=/dev/null
 ac_configure_args="$ac_configure_args --host='$kpse_build_alias' \
 CC='$BUILDCC' CPPFLAGS='$BUILDCPPFLAGS'\
 CFLAGS='$BUILDCFLAGS' LDFLAGS='$BUILDLDFLAGS'"])])

AC_OUTPUT
