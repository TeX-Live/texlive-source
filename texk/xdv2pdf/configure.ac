dnl Process this file with autoconf to produce a configure script.
dnl
dnl   Copyright (C) 2009, 2010 Peter Breitenlohner <tex-live@tug.org>
dnl
dnl   This file is free software; the copyright holder
dnl   gives unlimited permission to copy and/or distribute it,
dnl   with or without modifications, as long as this notice is preserved.
dnl
AC_INIT([xdv2pdf], [0.998], [tex-k@tug.org])
AC_PREREQ([2.65])
AC_CONFIG_SRCDIR([xdv2pdf.cpp])
AC_CONFIG_AUX_DIR([../../build-aux])
AC_CONFIG_MACRO_DIR([../../m4])

dnl Common code for all programs using libkpathsea.
KPSE_COMMON([xdv2pdf])

AC_CONFIG_HEADERS([config.h])

dnl Checks for programs.
AC_PROG_CXX
AC_PROG_OBJC

KPSE_KPATHSEA_FLAGS

dnl First, check if we're configuring on Mac OS X -- can't build elsewhere

KPSE_CHECK_FRAMEWORK([Carbon], [ATSUStyle style])
KPSE_CHECK_FRAMEWORK([QuickTime], [ImageDescriptionHandle h])

if test "x$kpse_cv_have_Carbon:$kpse_cv_have_QuickTime" = xyes:yes; then
  can_build=yes
else
  AC_MSG_WARN([Carbon or QuickTime framework not found, can not build xdv2pdf.])
  can_build=no
fi

if test "x$can_build" = xyes; then

dnl Checks for typedefs, structures, and compiler characteristics.
AC_STRUCT_TM
AC_C_BIGENDIAN

dnl Checks for library functions.
AC_CHECK_FUNCS([strspn system])
AC_SEARCH_LIBS([pow], [m])

if test "x$enable_build" != xno || test -f config.force; then

dnl Check that Kpathsea supports the opentype file format.
KPSE_ADD_FLAGS([kpathsea])
KPSE_CHECK_KPSE_FORMAT([opentype], [],
                       [AC_MSG_ERROR([Kpathsea does not support opentype files, can not build xdv2pdf.])])
KPSE_RESTORE_FLAGS

echo timestamp >config.force
fi

fi

AM_CONDITIONAL([build], [test "x$can_build" = xyes])

AC_CONFIG_FILES([Makefile])

AC_OUTPUT
