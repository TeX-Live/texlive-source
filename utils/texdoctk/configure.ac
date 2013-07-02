dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2012, 2013 Karl Berry <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([texdoctk (TeX Live)], [0.6.1], [tex-live@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([texdoctk.pl])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

KPSE_BASIC([texdoctk])

# All scripts need these, apparently.
AC_PROG_LN_S

KPSE_COND_WIN32_WRAP

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
