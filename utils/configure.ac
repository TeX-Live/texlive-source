dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2013-2025 Karl Berry <tex-live@tug.org>
dnl   Copyright 2009-2013 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([../version.ac])[] dnl define tex_live_version
AC_INIT([TeX Live utils], tex_live_version(), [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([../build-aux/missing])
AC_CONFIG_AUX_DIR([../build-aux])
AC_CONFIG_MACRO_DIRS([../m4])

# utils/configure.ac: kpse_setup call; see comment in ../configure.ac.
KPSE_SETUP([../])

dnl This is an example of how we might enable an autoconf-only package,
dnl which will not typically have an ac/withenable.ac file.  We could
dnl add it to their source tree, or make the call here.
dnl
dnl # utils/configure.ac: enable autoconf (not automake) bibcheck.
dnl # Has to come after kpse_setup because that is what sets enable_all_pkgs,
dnl # which defines the default for whether a given package is enabled.
dnl KPSE_ENABLE_PROG([bibcheck])

KPSE_BASIC([utils])

AC_PROG_MKDIR_P

KPSE_RECURSE_PROGS([utils], [Utils])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
