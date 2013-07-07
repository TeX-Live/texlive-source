dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([../version.ac])[] dnl define tex_live_version
AC_INIT([TeX Live texk], tex_live_version(), [peb@mppmu.mpg.de])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([../build-aux/missing])
AC_CONFIG_AUX_DIR([../build-aux])
AC_CONFIG_MACRO_DIR([../m4])

dnl See comment in ../configure.ac
KPSE_SETUP([../])

KPSE_BASIC([texk])

AC_PROG_MKDIR_P

AC_MSG_CHECKING([for TeXk programs to build])
MAKE_SUBDIRS=
CONF_SUBDIRS=
KPSE_FOR_PKGS([texk], [
if test -x $srcdir/Kpse_Pkg/configure; then
  test "x$enable_[]Kpse_pkg" = xno || MAKE_SUBDIRS="$MAKE_SUBDIRS Kpse_Pkg"
dnl Cause 'make dist' to recurse into all subtexk dirs
  CONF_SUBDIRS="$CONF_SUBDIRS Kpse_Pkg"
dnl Cause 'configure -hr' and 'autoreconf' to recurse into all subtexk dirs
  if false; then
    AC_CONFIG_SUBDIRS(Kpse_Pkg)
  fi
fi
])
AC_SUBST([MAKE_SUBDIRS])
AC_SUBST([CONF_SUBDIRS])
AC_MSG_RESULT([$MAKE_SUBDIRS])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
