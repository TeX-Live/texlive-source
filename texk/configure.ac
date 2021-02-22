dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2017-2020 Karl Berry <tex-live@tug.org>
dnl   Copyright 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([../version.ac])[] dnl define tex_live_version
AC_INIT([TeX Live texk], tex_live_version(), [tex-live@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([../build-aux/missing])
AC_CONFIG_AUX_DIR([../build-aux])
AC_CONFIG_MACRO_DIRS([../m4])

dnl See comment in ../configure.ac
KPSE_SETUP([../])

KPSE_BASIC([texk])

AC_PROG_MKDIR_P

KPSE_RECURSE_PROGS([texk], [TeXk])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
