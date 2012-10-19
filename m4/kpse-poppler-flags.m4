# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2011, 2012 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_POPPLER_FLAGS
# ---------------
# Provide the configure option '--with-system-poppler' (if in the TL tree).
#
# Set the make variables POPPLER_INCLUDES and POPPLER_LIBS to the CPPFLAGS and
# LIBS required for the `-lpoppler' library in libs/poppler/ of the TL tree.
AC_DEFUN([KPSE_POPPLER_FLAGS],
[AC_REQUIRE([KPSE_CHECK_WIN32])[]dnl
_KPSE_LIB_FLAGS([poppler], [poppler], [],
                [-IBLD/libs/poppler -IBLD/libs/poppler/goo -IBLD/libs/poppler/poppler],
                [BLD/libs/poppler/libpoppler.a], [],
                [], [${top_builddir}/../../libs/poppler/poppler/Stream.h])[]dnl
test "x$kpse_cv_have_win32" = xno || POPPLER_LIBS="$POPPLER_LIBS -lgdi32"
]) # KPSE_POPPLER_FLAGS

# KPSE_POPPLER_OPTIONS([WITH-SYSTEM])
# --------------------------------
AC_DEFUN([KPSE_POPPLER_OPTIONS],
[m4_ifval([$1],
          [AC_ARG_WITH([system-poppler],
                       AS_HELP_STRING([--with-system-poppler],
                                      [use installed poppler headers and library (requires pkg-config)]))])[]dnl
]) # KPSE_POPPLER_OPTIONS

# KPSE_POPPLER_SYSTEM_FLAGS
# ----------------------
AC_DEFUN([KPSE_POPPLER_SYSTEM_FLAGS],
[AC_REQUIRE([_KPSE_CHECK_PKG_CONFIG])[]dnl
if $PKG_CONFIG poppler --atleast-version=0.12; then
  POPPLER_VERSION='-DPOPPLER_VERSION=\"'`$PKG_CONFIG poppler --modversion`'\"'
  POPPLER_INCLUDES="$POPPLER_VERSION `$PKG_CONFIG poppler --cflags`"
  POPPLER_LIBS=`$PKG_CONFIG poppler --libs`
elif test "x$need_poppler:$with_system_poppler" = xyes:yes; then
  AC_MSG_ERROR([did not find poppler-0.12 or better])
fi
]) # KPSE_POPPLER_SYSTEM_FLAGS
