dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([../version.ac])[] dnl define gmp_version
AC_INIT([gmp native], gmp_version, [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([../gmp-src/gmp-impl.h])
AC_CONFIG_AUX_DIR([../../../build-aux])
AC_CONFIG_MACRO_DIR([../../../m4])

KPSE_BASIC([gmp native])

AC_PROG_CC
AC_PROG_LN_S

KPSE_SEARCH_LIBS([MATH_LIB], [pow], [m])

AC_SUBST([GMP_TREE], [../gmp-src])

dnl Not used but avoids putting useless -D's on the command line.
AC_CONFIG_HEADERS([config.h])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
