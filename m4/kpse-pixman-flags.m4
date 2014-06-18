# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2012-2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_PIXMAN_FLAGS
# -----------------
# Provide the configure options '--with-system-pixman' (if in the TL tree).
#
# Set the make variables PIXMAN_INCLUDES and PIXMAN_LIBS to the CPPFLAGS and
# LIBS required for the `-lpixman-1' library in libs/pixman/ of the TL tree.
AC_DEFUN([KPSE_PIXMAN_FLAGS], [dnl
_KPSE_LIB_FLAGS([pixman], [pixman], [],
                [-IBLD/libs/pixman/include], [BLD/libs/pixman/libpixman.a], [],
                [], [${top_builddir}/../../libs/pixman/include/pixman.h])[]dnl
]) # KPSE_PIXMAN_FLAGS

# KPSE_PIXMAN_OPTIONS([WITH-SYSTEM])
# ----------------------------------
AC_DEFUN([KPSE_PIXMAN_OPTIONS], [_KPSE_LIB_OPTIONS([pixman], [$1], [pkg-config])])

# KPSE_PIXMAN_SYSTEM_FLAGS
# ------------------------
AC_DEFUN([KPSE_PIXMAN_SYSTEM_FLAGS], [dnl
AC_REQUIRE([_KPSE_CHECK_PKG_CONFIG])[]dnl
if $PKG_CONFIG pixman-1 --atleast-version=0.18; then
  PIXMAN_INCLUDES=`$PKG_CONFIG pixman-1 --cflags`
  PIXMAN_LIBS=`$PKG_CONFIG pixman-1 --libs`
elif test "x$need_pixman:$with_system_pixman" = xyes:yes; then
  AC_MSG_ERROR([did not find pixman-0.18 or better])
fi
]) # KPSE_PIXMAN_SYSTEM_FLAGS
