dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define dvi2tty_version
AC_INIT([dvi2tty (TeX Live)], dvi2tty_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([dvi2tty-src/dvi2tty.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([dvi2tty])

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

KPSE_KPATHSEA_FLAGS
KPSE_PTEXENC_FLAGS

AC_SUBST([DVI2TTY_TREE], [dvi2tty-src])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
