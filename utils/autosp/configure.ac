dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2016-2020 Karl Berry <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([autosp_version], [2020-03-11])[]dnl using unmodified autosp source
AC_INIT([autosp (TeX Live)], autosp_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([autosp-src/autosp.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([autosp])

# Copied from autosp-src/configure.ac.
# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for header files.
AC_CHECK_HEADERS([stdlib.h string.h stdio.h time.h getopt.h ctype.h limits.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_HEADER_STDBOOL
AC_TYPE_SIZE_T

# Checks for library functions.
AC_CHECK_FUNCS([strstr])
# End of checks from autosp-src/configure.ac.

# trailing tl stuff.
AC_PROG_MAKE_SET
KPSE_COND_WIN32_WRAP
AC_SUBST([AUTOSP_TREE], [autosp-src])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
