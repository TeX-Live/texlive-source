dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2016 Karl Berry <tex-live@tug.org>
dnl   Copyright 2010-2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([upmendex (TeX Live)], [0.52])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AC_PROG_CXX
KPSE_CXX_HACK

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([upmendex], [no-define])

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

m4_include([../../version.ac])[] dnl define tex_live_version
AC_DEFINE([TL_VERSION], ["TeX Live tex_live_version()"],
          [Define to the current TeX Live version string.])

AC_CACHE_CHECK([if C99 variadic macros are supported],
               [kpse_cv_have___VA_ARGS__],
               [AC_COMPILE_IFELSE([AC_LANG_PROGRAM([[#include <stdio.h>
#define foo(format, ...) printf(format, __VA_ARGS__)]],
                                                   [[foo("%s\n", "Hi, there");]])],
                                  [kpse_cv_have___VA_ARGS__=yes],
                                  [kpse_cv_have___VA_ARGS__=no])])
AS_CASE([$kpse_cv_have___VA_ARGS__],
        [yes], [AC_DEFINE([HAVE___VA_ARGS__], 1,
                          [Define to 1 if you have C99 variadic macros.])])

KPSE_KPATHSEA_FLAGS
KPSE_ICU_FLAGS([icuio icui18n], [--ldflags-icuio])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
