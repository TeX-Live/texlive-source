dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2011-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([ttf2pk2 (TeX Live)], [2.0], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([ttf2pk.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([ttf2pk], [no-define])

AC_CONFIG_HEADERS([config.h])

AC_SEARCH_LIBS([floor], [m])

KPSE_KPATHSEA_FLAGS
KPSE_FREETYPE2_FLAGS

if test "x$enable_build" != xno || test -f config.force; then

KPSE_ADD_FLAGS([kpathsea])
AC_CHECK_HEADER([kpathsea/version.h], [],
                [AC_MSG_ERROR([Sorry, kpathsea too old])])
KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
