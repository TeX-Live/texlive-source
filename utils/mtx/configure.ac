dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2012 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define mtx_version
AC_INIT([mtx for TeX Live], mtx_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([mtx-]mtx_version[/analyze.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

dnl Check compiler and flags
AC_PROG_CC

KPSE_COMPILER_WARNINGS

KPSE_CHECK_WIN32
AM_CONDITIONAL([WIN32], [test "x$kpse_cv_have_win32" != xno])
AM_CONDITIONAL([WIN32_WRAP],
               [test -r $srcdir/../../texk/texlive/w32_wrapper/runscript.exe])

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([memory.h stddef.h stdlib.h string.h strings.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_FUNC_ERROR_AT_LINE
dnl AC_FUNC_MALLOC
AC_FUNC_STRTOD
AC_CHECK_FUNCS([memchr memmove memset strchr strrchr strtol])

AC_SUBST([MTX_TREE], [mtx-]mtx_version)

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
