# $Id$
#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.
dnl   Copyright 2018 John Collins
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([axohelp (TeX Live)], [1.3], [jcc8@psu.edu])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([axodraw2-src/axohelp.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([axodraw2], [no-define])

AC_CHECK_LIB([m], [sqrt],,
  AC_MSG_ERROR([*** Please install libm on your system ***]))
AC_PROG_CC

# trailing TL stuff.
AC_PROG_MAKE_SET
KPSE_COND_WIN32_WRAP
AC_SUBST([AXODRAW2_TREE], [axodraw2-src])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
