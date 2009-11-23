dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([TeX Live libs], [2009], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([../build-aux/missing])
AC_CONFIG_AUX_DIR([../build-aux])
AC_CONFIG_MACRO_DIR([../m4])

dnl See comment in ../configure.ac
KPSE_SETUP([../])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

AC_PROG_MKDIR_P

AC_MSG_CHECKING([for libraries to build])
SUBLIBS=
DIST_SUBLIBS=
dnl XeTeX needs the ICU headers and libraries from the TL tree,
dnl whereas bibtexu may use installed ICU headers and libraries.
test "x$need_icu_xetex" = xyes && need_icu=yes && with_system_icu=no
KPSE_FOR_PKGS([libs], [
if test -x $srcdir/Kpse_Pkg/configure; then
  if test "x$with_system_[]Kpse_pkg" != xyes && \
     test "x$need_[]Kpse_pkg" = xyes; then
    SUBLIBS="Kpse_Pkg $SUBLIBS"    
  fi
dnl Cause 'make dist' to recurse into all sublibs dirs
  DIST_SUBLIBS="Kpse_Pkg $DIST_SUBLIBS"
dnl Cause 'configure -hr' and 'autoreconf' to recurse into all sublibs dirs
  if false; then
    AC_CONFIG_SUBDIRS(Kpse_Pkg)
  fi
fi
])
AC_SUBST([SUBLIBS])
AC_SUBST([DIST_SUBLIBS])
AC_MSG_RESULT([$SUBLIBS])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
