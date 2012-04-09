dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2010-2012 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
dnl *********************************************************************
dnl
dnl  Adapted for TeX Live from chktex-*/configure.in
dnl
dnl *********************************************************************
dnl
m4_define([chktex_version], [1.6.6])[]dnl using unmodified chktex source tree
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
AC_PROG_MAKE_SET
KPSE_CHECK_PDFLATEX

dnl No termlib/termcap/ncurses library for TeX Live

dnl Checks for header files.
AC_HEADER_STDC
AC_HEADER_DIRENT
AC_CHECK_HEADERS([limits.h stat.h strings.h stdarg.h sys/stat.h unistd.h])

dnl Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_C_INLINE
AC_TYPE_SIZE_T

dnl Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([access closedir fileno isatty opendir readdir stat strcasecmp strdup strlwr strtol])

dnl Rebuild ChkTeX.pdf if required and amintainer-mode is enabled.
test "x$enable_maintainer_mode" = xyes && PDF_DEPEND=ChkTeX.tex
AC_SUBST([PDF_DEPEND])

AC_CONFIG_HEADERS([config.h])
AC_DEFINE([TEX_LIVE], 1, [Define to 1 for a TeX Live build.])

KPSE_CHECK_WIN32
AM_CONDITIONAL([WIN32], [test "x$kpse_cv_have_win32" != xno])
AM_CONDITIONAL([WIN32_WRAP],
               [test -r $srcdir/../../texk/texlive/w32_wrapper/runscript.exe])

AC_SUBST([CHKTEX_TREE], [chktex-]chktex_version)
AC_SUBST([PERL5], ['/usr/bin/env perl'])

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([ChkTeX.tmp:chktex-]chktex_version[/ChkTeX.tex.in])
AC_CONFIG_FILES([deweb.pl:chktex-]chktex_version[/deweb.in], [chmod +x deweb.pl])

AC_OUTPUT
