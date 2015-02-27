dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([SeeTeX (TeX Live)], [1.1], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([dviselect.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([seetexk])

AC_CONFIG_HEADERS([config.h])

AC_CHECK_FUNCS([strerror])
AC_CHECK_HEADERS([errno.h])

AC_TYPE_INT16_T
AC_TYPE_INT32_T
AC_TYPE_UINT32_T

KPSE_KPATHSEA_FLAGS

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
