dnl Process this file with Autoconf to produce a configure script for Web2c.
dnl
dnl   Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
dnl --------------------------------------------------------
dnl
dnl This directory is configured separately, because here we create tools
dnl that have to run on the build system.
dnl
m4_include([../../../version.ac])[] dnl define tex_live_version
AC_INIT([Web2C Tools], tex_live_version(), [tex-k@tug.org])
AC_PREREQ([2.63])
dnl 
dnl We don't use (for example) tex/tex.web because people who only want
dnl to build part of the distribution may not have any given program.
dnl Even main.c isn't guaranteed, but then nothing is, really ...
AC_CONFIG_SRCDIR([main.c])
AC_CONFIG_AUX_DIR([../../../build-aux])
AC_CONFIG_MACRO_DIR([../../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([web2c convert])

AC_PROG_YACC
KPSE_PROG_LEX

AC_CHECK_FUNCS([strerror])
AC_CHECK_HEADERS([errno.h])
AC_CHECK_SIZEOF([int])
AC_CHECK_SIZEOF([long])
AC_CHECK_SIZEOF([void *])
AC_TYPE_UINTPTR_T

dnl Check if <ctype.h> declares isascii.
AC_CHECK_DECLS([isascii], [], [], [[#include <ctype.h>]])

dnl Write output here, instead of putting a zillion -D's on the command line.
AC_CONFIG_HEADERS([kpathsea/c-auto.h:c-auto.in])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
