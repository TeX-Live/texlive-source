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

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_MAKE_SET

dnl Determine the TeX Live platform name (not yet, just prepare)
KPSE_TL_PLATFORM

dnl Check if we have an executable for the current platform.
AC_MSG_CHECKING([for a biber binary])
biber_bin="bin/$TL_PLATFORM/biber"
if test -r "$srcdir/$biber_bin"; then
  BIBER_BINARY="$biber_bin"
elif test -r "$srcdir/$biber_bin.exe"; then
  BIBER_BINARY="$biber_bin.exe"
else
  BIBER_BINARY=
fi
AC_MSG_RESULT([$BIBER_BINARY])
AC_SUBST([BIBER_BINARY])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
