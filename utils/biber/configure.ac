dnl See ./README for our basic strategy here.
dnl
dnl   Copyright (C) 2011 Karl Berry <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([Biber], [TeX Live], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([TeXLive/TLConfig.pm])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

# System type for cross-compilation.
AC_CANONICAL_HOST
AC_EXEEXT

# Checks for programs.
AC_PROG_MAKE_SET

AC_MSG_CHECKING([for TeX Live platform name])
# we need these variables in the Perl.
export ac_aux_dir
export srcdir
#
TL_platform=`perl -e '
  unshift (@INC, "$ENV{srcdir}");
  use TeXLive::TLUtils;
  print TeXLive::TLUtils::platform();'`
#
AC_MSG_RESULT([$TL_platform])
if test -z "$TL_platform"; then
  AC_MSG_ERROR("cannot determine TeX Live platform name")
fi

biber_exe=$srcdir/bin/$TL_platform/biber$ac_exeext
AC_MSG_CHECKING([for executable in $biber_exe])
if test -r "$biber_exe"; then
  AC_SUBST([BIBER_EXE], [$biber_exe])
  AC_MSG_RESULT([yes])
else
  AC_MSG_RESULT([no])
fi

# Check for having an executable for the needed platform.
AM_CONDITIONAL([TL_HAVE_BIBER_BINARY], [test -r "$biber_exe"])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
