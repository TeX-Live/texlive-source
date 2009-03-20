dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([tex4ht for TeX Live], [20080225], [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([tex4ht.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([tex4htk])

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

AC_CHECK_SIZEOF([int])

KPSE_KPATHSEA_FLAGS

tex4ht_subdir='texmf-dist/scripts/tex4ht'
tex4htdir="\${prefix}/$tex4ht_subdir"
AC_SUBST([tex4htdir])
dnl This is not quite right, should actually be decided by 'make install'
if test "x$bindir" = 'x${exec_prefix}/bin'; then
  tex4htrel="../$tex4ht_subdir"
else
  tex4htrel="../../$tex4ht_subdir"
fi
AC_SUBST([tex4htrel])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
