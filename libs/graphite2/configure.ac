dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2012, 2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define graphite2_version
AC_INIT([graphite2 (TeX Live)], graphite2_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([graphite2-src/graphite2.pc.in])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([graphite2])

dnl ! C++11 required since graphite2 1.3.12 !
AC_PROG_CC
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX([11])
AC_PROG_RANLIB
AC_PROG_LN_S

AC_CONFIG_HEADERS([config.h])

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

AM_CONDITIONAL([DIRECT_TYPE], [test "x$GCC" = xyes])

enable_fileface=yes
AM_CONDITIONAL([WITH_FILEFACE], [test "x$enable_fileface" != xno])
if test "x$enable_fileface" = no; then
  AC_DEFINE([GRAPHITE2_NFILEFACE], 1,
            [Define to 1 to disable fileface support.])
fi

enable_tracing=no
AM_CONDITIONAL([WITH_TRACING], [test "x$enable_tracing" != xno])
if test "x$enable_tracing" = no; then
  AC_DEFINE([GRAPHITE2_NTRACING], 1,
            [Define to 1 to disable tracing support.])
fi

AC_SUBST([GRAPHITE2_TREE], [graphite2-src])

AC_CONFIG_FILES([Makefile include/graphite2/Makefile])

AC_OUTPUT
