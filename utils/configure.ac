dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([TeX Live utils], [2009], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([../build-aux/missing])
AC_CONFIG_AUX_DIR([../build-aux])
AC_CONFIG_MACRO_DIR([../m4])

dnl See comment in ../configure.ac
KPSE_SETUP([../])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

AC_PROG_MKDIR_P

AC_MSG_CHECKING([for Utils programs to build])
SUBUTILS=
DIST_SUBUTILS=
KPSE_FOR_PKGS([utils], [
if test -x $srcdir/Kpse_Pkg/configure; then
  test "x$enable_[]Kpse_pkg" = xno || SUBUTILS="$SUBUTILS Kpse_Pkg"
dnl Cause 'make dist' to recurse into all subutils dirs
  DIST_SUBUTILS="$DIST_SUBUTILS Kpse_Pkg"
dnl Cause 'configure -hr' and 'autoreconf' to recurse into all subutils dirs
  if false; then
    AC_CONFIG_SUBDIRS(Kpse_Pkg)
  fi
fi
])
AC_SUBST([SUBUTILS])
AC_SUBST([DIST_SUBUTILS])
AC_MSG_RESULT([$SUBUTILS])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
