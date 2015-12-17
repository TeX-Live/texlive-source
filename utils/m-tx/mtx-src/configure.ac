dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2012 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define mtx_version
AC_INIT([mtx], mtx_version, [rdt@cs.queensu.ca])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([analyze.c])

AM_INIT_AUTOMAKE(mtx, mtx_version)

dnl Check compiler and flags
AC_PROG_CC

AM_CONDITIONAL([WIN32], [test "x$kpse_cv_have_win32" != xno])

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AC_PROG_RANLIB

# Checks for libraries.

# Checks for header files.
AC_CHECK_HEADERS([memory.h stddef.h stdlib.h string.h strings.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([memchr memmove memset strchr strrchr strtol])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
