dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2010 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
dnl *********************************************************************
dnl
dnl  Adapted for TeX Live from chktex-1.6.4/configure.in
dnl
dnl *********************************************************************
dnl
m4_define([chktex_version], [1.6.4])[]dnl using unmodified chktex source tree
AC_INIT([ChkTeX for TeX Live], chktex_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([chktex-]chktex_version[/ChkTeX.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

dnl Check compiler and flags
AC_PROG_CC

KPSE_COMPILER_WARNINGS

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_LN_S

dnl Checks for libraries.
AC_CHECK_LIB(termlib, tgetent)
AC_CHECK_LIB(termcap, tgetent)

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_DIRENT
AC_CHECK_HEADERS(limits.h stat.h strings.h stdarg.h dnl
sys/stat.h termcap.h termlib.h unistd.h)

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS(access closedir fileno isatty opendir readdir stat dnl
strcasecmp strdup strlwr strtol)

AC_CONFIG_HEADERS([config.h])

CHKTEX_TREE=[chktex-]chktex_version
AC_SUBST([CHKTEX_TREE])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
