dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009, 2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([gsftopk (TeX Live)], [1.19.2], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([gsftopk.c])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([gsftopk])

AC_CONFIG_HEADERS([c-auto.h:c-auto.in])

dnl Checks for header files.
AC_HEADER_SYS_WAIT
AC_HEADER_TIME
AC_CHECK_HEADERS([poll.h sys/time.h])

dnl Checks for library functions.
AC_FUNC_VFORK
AC_TYPE_SIGNAL
AC_CHECK_FUNCS([poll sigaction])

if test "$ac_cv_header_poll_h" = no || test "$ac_cv_func_poll" = no; then
  AC_CHECK_HEADERS([sys/select.h select.h])
fi

KPSE_KPATHSEA_FLAGS

if test "x$enable_build" != xno || test -f config.force; then

KPSE_ADD_FLAGS([kpathsea])
AC_CHECK_HEADER([kpathsea/version.h], [],
                [AC_MSG_ERROR([Sorry, kpathsea too old])])
KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
