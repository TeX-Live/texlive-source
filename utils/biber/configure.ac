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
AC_CONFIG_SRCDIR([biber.i386-linux])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign])
AM_MAINTAINER_MODE

dnl Checks for programs.
AC_PROG_INSTALL
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
AC_MSG_RESULT($TL_platform)
if test -z "$TL_platform"; then
  AC_MSG_ERROR("cannot determine TeX Live platform name")
fi
#
AC_SUBST([TL_PLATFORM], [$TL_platform])

# Check if we have an executable for the current platform.
AM_CONDITIONAL([TL_HAVE_BIBER_BINARY],
   [test -r "$srcdir/biber.$TL_platform"])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
