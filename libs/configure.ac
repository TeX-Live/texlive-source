dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([../version.ac])[] dnl define tex_live_version
AC_INIT([TeX Live libs], tex_live_version(), [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([../build-aux/missing])
AC_CONFIG_AUX_DIR([../build-aux])
AC_CONFIG_MACRO_DIR([../m4])

dnl See comment in ../configure.ac
KPSE_SETUP([../])

KPSE_BASIC([libs])

AC_PROG_MKDIR_P

AC_MSG_CHECKING([for generic libraries to build])
SUBLIBS=
DIST_SUBLIBS=
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
