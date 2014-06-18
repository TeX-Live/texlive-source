# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009-2014 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_LIBPNG_FLAGS
# -----------------
# Provide the configure options '--with-system-libpng' (if in the TL tree).
#
# Set the make variables LIBPNG_INCLUDES and LIBPNG_LIBS to the CPPFLAGS and
# LIBS required for the `-lpng' library in libs/libpng/ of the TL tree.
AC_DEFUN([KPSE_LIBPNG_FLAGS],
[AC_REQUIRE([KPSE_ZLIB_FLAGS])[]dnl
_KPSE_LIB_FLAGS([libpng], [png], [],
                [-IBLD/libs/libpng/include], [BLD/libs/libpng/libpng.a], [],
                [], [${top_builddir}/../../libs/libpng/include/png.h])[]dnl
]) # KPSE_LIBPNG_FLAGS

# KPSE_LIBPNG_OPTIONS([WITH-SYSTEM])
# ----------------------------------
AC_DEFUN([KPSE_LIBPNG_OPTIONS], [_KPSE_LIB_OPTIONS([libpng], [$1], [pkg-config])])

# KPSE_LIBPNG_SYSTEM_FLAGS
# ------------------------
AC_DEFUN([KPSE_LIBPNG_SYSTEM_FLAGS],
[AC_REQUIRE([_KPSE_CHECK_PKG_CONFIG])[]dnl
if $PKG_CONFIG libpng; then
  LIBPNG_INCLUDES=`$PKG_CONFIG libpng --cflags`
  LIBPNG_LIBS=`$PKG_CONFIG libpng --libs`
elif test "x$need_libpng:$with_system_libpng" = xyes:yes; then
  AC_MSG_ERROR([did not find libpng])
fi
]) # KPSE_LIBPNG_SYSTEM_FLAGS
