dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2010-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
dnl *********************************************************************
dnl
dnl  Adapted for TeX Live from chktex-src/configure.in
dnl
dnl *********************************************************************
dnl
m4_include([version.ac])[] dnl define chktex_version
AC_INIT([ChkTeX (TeX Live)], chktex_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([chktex-src/ChkTeX.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([chktex])

dnl Need latex, dvips, and ps2pdf to build ChkTeX.pdf
KPSE_CHECK_LATEX
AC_CHECK_PROGS([DVIPS], [dvips], [no])
AC_CHECK_PROGS([PS2PDF], [ps2pdf], [no])

dnl Checks for header files.
AC_CHECK_HEADERS([stat.h stdarg.h sys/stat.h])

dnl Checks for library functions.
AC_FUNC_VPRINTF
AC_CHECK_FUNCS([access closedir fileno isatty opendir readdir stat strcasecmp strdup strlwr])

dnl Checks for declarations.
AC_CHECK_DECLS([stpcpy])

AC_CONFIG_HEADERS([config.h])

dnl For TeX Live we always use Posix extended regular expressions
AC_DEFINE([TEX_LIVE], 1, [Define to 1 for a TeX Live build.])
AC_DEFINE([HAVE_PCRE], 0, [Whether PCRE can be used for user warnings.])
AC_DEFINE([HAVE_POSIX_ERE], 1, [Whether POSIX ERE can be used for user warnings.])

KPSE_COND_MINGW32
KPSE_COND_WIN32_WRAP

AC_SUBST([CHKTEX_TREE], [chktex-src])
AC_SUBST([PERL5], ['/usr/bin/env perl'])

KPSE_KPATHSEA_FLAGS

AC_CONFIG_FILES([Makefile ChkTeX.t:chktex-src/ChkTeX.tex.in])
AC_CONFIG_FILES([deweb.pl:chktex-src/deweb.in], [chmod +x deweb.pl])

AC_OUTPUT
