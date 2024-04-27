dnl Process this file with autoconf to produce a configure script.
dnl
dnl Copyright 2018-2024 Japanese TeX Development Community <issue@texjp.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([dviout-util (TeX Live)], [20240427], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([dvispc.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([dviout-util])

AC_CONFIG_HEADERS([config.h])

m4_include([../../version.ac])[] dnl define tex_live_version
AC_DEFINE([TL_VERSION], ["TeX Live tex_live_version()"],
          [Define to the current TeX Live version string.])

KPSE_KPATHSEA_FLAGS
KPSE_PTEXENC_FLAGS

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
