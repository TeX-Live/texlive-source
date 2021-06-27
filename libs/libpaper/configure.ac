dnl $Id$
dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright 2015-2021 Karl Berry <tex-live@tug.org>
dnl   Copyright 2013-2014 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_include([version.ac])[] dnl define libpaper_version
AC_INIT([libpaper (TeX Live)], libpaper_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([libpaper-src/lib/paper.h])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIRS([../../m4])

dnl Must come before KPSE_BASIC to avoid Autoconf 2.70 warning:
dnl   AC_LINK_IFELSE was called before AC_USE_SYSTEM_EXTENSIONS
AC_USE_SYSTEM_EXTENSIONS

KPSE_BASIC([paper])

AC_PROG_CC
AC_PROG_RANLIB
AC_PROG_LN_S

# So we can use reasonable awk in the Makefile.
AC_PROG_AWK

AC_CONFIG_HEADERS([config.h])

AM_CONDITIONAL([build], [test "x$enable_build" != xno])

if test "x$enable_build" != xno || test -f config.force; then

echo timestamp >config.force
fi

AC_SUBST([PAPERSIZE], [a4])
AC_DEFINE_UNQUOTED([PAPERSIZE], ["$PAPERSIZE"], [Default paper size])

AC_SUBST([PAPERSIZEVAR], [PAPERSIZE])
AC_DEFINE_UNQUOTED([PAPERSIZEVAR], ["$PAPERSIZEVAR"],
                   [Environment variable that overrides the default paper size])

AC_SUBST([LIBPAPER_TREE], [libpaper-src])

AC_CONFIG_FILES([Makefile 
                include/Makefile
                paperconf.1:libpaper-src/man/paperconf.1.in])
AC_OUTPUT
