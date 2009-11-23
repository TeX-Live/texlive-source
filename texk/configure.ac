dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([TeX Live texk], [2009], [peb@mppmu.mpg.de])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([../build-aux/missing])
AC_CONFIG_AUX_DIR([../build-aux])
AC_CONFIG_MACRO_DIR([../m4])

dnl See comment in ../configure.ac
KPSE_SETUP([../])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

AC_PROG_MKDIR_P

AC_MSG_CHECKING([for TeXk programs to build])
SUBTEXK=
DIST_SUBTEXK=
KPSE_FOR_PKGS([texk], [
if test -x $srcdir/Kpse_Pkg/configure; then
  test "x$enable_[]Kpse_pkg" = xno || SUBTEXK="$SUBTEXK Kpse_Pkg"
dnl Cause 'make dist' to recurse into all subtexk dirs
  DIST_SUBTEXK="$DIST_SUBTEXK Kpse_Pkg"
dnl Cause 'configure -hr' and 'autoreconf' to recurse into all subtexk dirs
  if false; then
    AC_CONFIG_SUBDIRS(Kpse_Pkg)
  fi
fi
])
AC_SUBST([SUBTEXK])
AC_SUBST([DIST_SUBTEXK])
AC_MSG_RESULT([$SUBTEXK])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
