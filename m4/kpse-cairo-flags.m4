# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2012-2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_CAIRO_FLAGS
# ----------------
# Provide the configure options '--with-system-pixman' (if in the TL tree).
#
# Set the make variables CAIRO_INCLUDES and CAIRO_LIBS to the CPPFLAGS and
# LIBS required for the `-lcairo' library in libs/cairo/ of the TL tree.
AC_DEFUN([KPSE_CAIRO_FLAGS], [dnl
_KPSE_LIB_FLAGS([cairo], [cairo], [],
                [-IBLD/libs/cairo/cairo], [BLD/libs/cairo/libcairo.a], [],
                [], [${top_builddir}/../../libs/cairo/cairo/cairo.h])[]dnl
]) # KPSE_CAIRO_FLAGS

# KPSE_CAIRO_OPTIONS([WITH-SYSTEM])
# ---------------------------------
AC_DEFUN([KPSE_CAIRO_OPTIONS], [_KPSE_LIB_OPTIONS([cairo], [$1], [pkg-config])])

# KPSE_CAIRO_SYSTEM_FLAGS
# -----------------------
AC_DEFUN([KPSE_CAIRO_SYSTEM_FLAGS], [dnl
AC_REQUIRE([_KPSE_CHECK_PKG_CONFIG])[]dnl
if $PKG_CONFIG cairo --atleast-version=1.12; then
  CAIRO_INCLUDES=`$PKG_CONFIG cairo --cflags`
  CAIRO_LIBS=`$PKG_CONFIG cairo --libs`
elif test "x$need_cairo:$with_system_cairo" = xyes:yes; then
  AC_MSG_ERROR([did not find cairo-1.12 or better])
fi
]) # KPSE_CAIRO_SYSTEM_FLAGS
