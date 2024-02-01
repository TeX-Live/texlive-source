dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2022-2024 Luigi Scarso <luigi.scarso@gmail.com>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define mpfi_version
AC_INIT([mpfi (TeX Live)], mpfi_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([mpfi-src/src/mpfi.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIRS([../../m4 m4])

KPSE_BASIC([mpfi])

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

KPSE_COMPILER_VISIBILITY

AC_CONFIG_HEADERS([mpfi_config.h])

KPSE_CANONICAL_HOST

MPFI_CONFIGS

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

KPSE_GMP_FLAGS
KPSE_MPFR_FLAGS

dnl Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T



if test "x$enable_build" != xno || test -f config.force; then

AC_CHECK_HEADER([limits.h], [], [AC_MSG_ERROR([limits.h not found])])
AC_CHECK_HEADER([float.h], [],  [AC_MSG_ERROR([float.h not found])])
AC_CHECK_HEADER([string.h], [], [AC_MSG_ERROR([string.h not found])])

KPSE_ADD_FLAGS([gmp])
KPSE_ADD_FLAGS([mpfr])


dnl Checks for library functions.
AC_CHECK_FUNCS([dup2 gettimeofday])
AC_CHECK_FUNCS([mpfr_q_sub mpfr_z_sub mpfr_z_div])


KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

AC_SUBST([MPFI_TREE], [mpfi-src])

dnl AC_CONFIG_LINKS([mparam.h:mpfi-src/src/mparam_h.in])

AC_CONFIG_FILES([Makefile include/Makefile])

AC_OUTPUT
