dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2026 Karl Berry <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([utfpatgen_version], [1.0])[]dnl
AC_INIT([utfpatgen (TeX Live)], utfpatgen_version, [tex-live@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([utfpatgen-src/utfpatgen.w])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([utfpatgen], [no-define])

# Checks for programs.
AC_PROG_CC

# In the similar utils/vlna program, peb set up a conditional and other
# support to rerun cweave and ctangle with --enable-maintainer-mode.
# Since that is essentially never(?) used nowadays, let's skip it.
# See the utfpatgen.c target in the Makefile[.am] instead.
AC_SUBST([UTFPATGEN_TREE], [utfpatgen-src])

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
