dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
dnl *********************************************************************
dnl
dnl  Adapted for TeX Live from gregorio-4.0.0-beta/configure.am
dnl
dnl *********************************************************************
dnl
m4_include([version.ac])[] dnl define gregorio_version
AC_INIT([ChkTeX (TeX Live)], gregorio_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([gregorio-]gregorio_version[/src/gregorio-utils.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([gregorio])

dnl Checks for header files.
AC_CHECK_HEADERS([stdalign.h])

AC_C_BIGENDIAN

AC_SUBST([GREGORIO_TREE], [gregorio-]gregorio_version)
AC_SUBST([PERL5], ['/usr/bin/env perl'])

KPSE_KPATHSEA_FLAGS

AC_CONFIG_HEADERS([config_.h])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
