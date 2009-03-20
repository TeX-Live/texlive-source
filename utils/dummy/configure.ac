dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([TeX Live utils dummy], [2009], [tex-k@tug.org])
AC_PREREQ([2.63])
AC_CONFIG_SRCDIR([../../build-aux/missing])
AC_CONFIG_AUX_DIR([../../build-aux])

AM_INIT_AUTOMAKE([foreign dist-bzip2])
AM_MAINTAINER_MODE

echo "$ac_configure_args" | sed 's,/dummy,/SUBUTILSDIR,g' >../subutilsdir-conf.args

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
