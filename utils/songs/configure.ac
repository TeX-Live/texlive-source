dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
dnl *********************************************************************
dnl
dnl  Adapted for TeX Live from songs-2.17/configure.ac
dnl
dnl *********************************************************************
dnl
m4_include([version.ac])[] dnl define songs_version
AC_INIT([songs (TeX Live)], songs_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([songs-]songs_version[/src/songidx/songidx.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([songs])

AC_PROG_CC
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
KPSE_CHECK_LATEX

AC_CHECK_HEADERS([locale.h stdlib.h stdio.h string.h wchar.h wctype.h])
AC_C_CONST
AC_TYPE_SIZE_T
AC_CHECK_FUNCS([setlocale strrchr])
AC_CHECK_TYPES([wchar_t], , ,[
#ifdef HAVE_STDLIB_H
#include <stdlib.h>
#endif
#ifdef HAVE_WCHAR_H
#include <wchar.h>
#endif
#ifdef HAVE_WCTYPE_H
#include <wctype.h>
#endif
])

AC_CONFIG_HEADERS([config.h])
AH_TOP([#ifndef CONFIG_H
#define CONFIG_H])
AH_BOTTOM([#endif])

AC_SUBST([SONGS_TREE], [songs-]songs_version)

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
