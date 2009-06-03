# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_FONTCONFIG_FLAGS
# ---------------------
# Provide the configure option '--with-fontconfig' and check for
# installed fontconfig headers and library.
# Check or set the cache variables kpse_cv_have_fontconfig,
# kpse_cv_fontconfig_includes, and kpse_cv_fontconfig_libs.
# If found set the Make variables FONTCONFIG_INCLUDES and FONTCONFIG_LIBS
# to the CPPFLAGS and LIBS required for the installed '-lfontconfig'
# library and define HAVE_LIBFONTCONFIG.
AC_DEFUN([KPSE_FONTCONFIG_FLAGS],
[AC_REQUIRE([_KPSE_CHECK_PKG_CONFIG])[]dnl
AC_ARG_WITH([fontconfig-includes],
            AS_HELP_STRING([--with-fontconfig-includes=DIR],
                           [fontconfig headers installed in DIR]))[]dnl
AC_ARG_WITH([fontconfig-libdir],
            AS_HELP_STRING([--with-fontconfig-libdir=DIR],
                           [fontconfig library installed in DIR]))[]dnl
AC_CACHE_CHECK([for installed fontconfig headers and library],
               [kpse_cv_have_fontconfig],
[kpse_save_CPPFLAGS=$CPPFLAGS
kpse_save_LIBS=$LIBS
kpse_cv_fontconfig_includes=
kpse_cv_fontconfig_libs='-lfontconfig'
if test "x$with_fontconfig_includes:$with_fontconfig_libdir" != x:; then
  if test "x$with_fontconfig_includes" != x; then
    kpse_cv_fontconfig_includes="-I$with_fontconfig_includes"
  fi
  if test "x$with_fontconfig_libdir" != x; then
    kpse_cv_fontconfig_libs="-L$with_fontconfig_libdir $kpse_cv_fontconfig_libs"
  fi
elif $PKG_CONFIG fontconfig; then
  kpse_cv_fontconfig_includes=`$PKG_CONFIG fontconfig --cflags`
  kpse_cv_fontconfig_libs=`$PKG_CONFIG fontconfig --libs`
fi
CPPFLAGS="$kpse_cv_fontconfig_includes $CPPFLAGS"
LIBS="$kpse_cv_fontconfig_libs $LIBS"
AC_LINK_IFELSE([AC_LANG_PROGRAM([[#include <fontconfig/fontconfig.h>]],
                                [[FcObjectSet *os; FcInit();]])],
               [kpse_cv_have_fontconfig=yes],
               [kpse_cv_have_fontconfig=no])
CPPFLAGS=$kpse_save_CPPFLAGS
LIBS=$kpse_save_LIBS])
if test "x$kpse_cv_have_fontconfig" = xyes; then
  FONTCONFIG_INCLUDES=$kpse_cv_fontconfig_includes
  FONTCONFIG_LIBS=$kpse_cv_fontconfig_libs
  AC_DEFINE([HAVE_LIBFONTCONFIG], 1, [Define if you have libfontconfig.])
fi
AC_SUBST([FONTCONFIG_INCLUDES])
AC_SUBST([FONTCONFIG_LIBS])
]) # KPSE_FONTCONFIG_FLAGS
