dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([musixflx_version], [0.83])[]dnl using unmodified musixflx source tree
AC_INIT([musixflx for TeX Live], musixflx_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([musixflx-]musixflx_version[/musixflx.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

AC_PROG_CC
KPSE_COMPILER_WARNINGS

AC_CONFIG_HEADERS([config.h])

MUSIXFLX_TREE=[musixflx-]musixflx_version
AC_SUBST([MUSIXFLX_TREE])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
