dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
m4_define([poppler_version], [0.12.4])[]dnl using unmodified poppler-0.12.4 source tree
AC_INIT([poppler for TeX Live], poppler_version, [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([poppler-]poppler_version[/poppler/Array.cc])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

AM_INIT_AUTOMAKE([foreign no-define])
AM_MAINTAINER_MODE

dnl ##### Optional features.
AC_ARG_ENABLE([a4-paper],
              AS_HELP_STRING([--enable-a4-paper],
                             [use A4 paper size instead of Letter for PostScript output]),
              [if test "x$enableval" = xyes; then
AC_DEFINE([A4_PAPER], 1, [Use A4 paper size instead of Letter for PostScript output])
fi
])

AC_ARG_ENABLE([opi],
              AS_HELP_STRING([--enable-opi],
                             [include support for OPI comments]),
              [if test "x$enableval" = xyes; then
AC_DEFINE([OPI_SUPPORT], 1, [Include support for OPI comments])
fi
])

AC_ARG_ENABLE([multithreaded],
              AS_HELP_STRING([--enable-multithreaded],
                             [include support for multithreading]),
              [if test "x$enableval" = xyes; then
AC_DEFINE([MULTITHREADED], 1, [Enable multithreading support])
fi
])

dnl **** Check for some programs and libraries ****

AC_PROG_CC
AC_PROG_CXX
AC_PROG_RANLIB
AC_PROG_LN_S

dnl **** Checks for header files ****

AC_CHECK_HEADERS([strings.h sys/select.h time.h])
AC_HEADER_DIRENT

dnl **** Checks for library functions ****

AC_CHECK_FUNCS([popen mkstemp mkstemps])
AC_SYS_LARGEFILE
AC_FUNC_FSEEKO

AC_CONFIG_HEADERS([aconf.h],
 [sed -e 's/^#define PACKAGE/#define POPPLER_PACKAGE/' \
      -e 's/^#define VERSION/#define POPPLER_VERSION/' aconf.h >aconf.tmp && mv -f aconf.tmp aconf.h])

AH_TOP([/*
 * aconf.h
 *
 * Copyright 2002-2003 Glyph & Cog, LLC
 */

#ifndef ACONF_H
#define ACONF_H

/*
 * Enable C++ exceptions.
 */
#undef USE_EXCEPTIONS])
AH_BOTTOM([#endif])

POPPLER_TREE=poppler-[]poppler_version
AC_SUBST([POPPLER_TREE])

AC_CONFIG_FILES([Makefile poppler/Makefile goo/Makefile fofi/Makefile])

AC_CONFIG_HEADERS([poppler-]poppler_version[/config.h])

AC_OUTPUT
