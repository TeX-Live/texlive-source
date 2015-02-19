# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# KPSE_TECKIT_FLAGS
# -----------------
# Provide the configure options '--with-system-teckit' (if in the TL tree).
#
# Set the make variables TECKIT_INCLUDES and TECKIT_LIBS to the CPPFLAGS and
# LIBS required for the `-lTECkit' library in libs/teckit/ of the TL tree.
AC_DEFUN([KPSE_TECKIT_FLAGS], [dnl
AC_REQUIRE([KPSE_ZLIB_FLAGS])[]dnl
_KPSE_LIB_FLAGS([teckit], [TECkit], [],
                [-IBLD/libs/teckit/include], [BLD/libs/teckit/libTECkit.a], [],
                [], [${top_builddir}/../../libs/teckit/include/teckit/TECkit_Common.h])[]dnl
]) # KPSE_TECKIT_FLAGS

# KPSE_TECKIT_OPTIONS([WITH-SYSTEM])
# ----------------------------------
AC_DEFUN([KPSE_TECKIT_OPTIONS], [_KPSE_LIB_OPTIONS([teckit], [$1], [pkg-config])])

# KPSE_TECKIT_SYSTEM_FLAGS
# -------------------------
AC_DEFUN([KPSE_TECKIT_SYSTEM_FLAGS], [dnl
AC_REQUIRE([_KPSE_CHECK_PKG_CONFIG])[]dnl
if $PKG_CONFIG teckit; then
  TECKIT_INCLUDES=`$PKG_CONFIG teckit --cflags`
  TECKIT_LIBS=`$PKG_CONFIG teckit --libs`
elif test "x$need_teckit:$with_system_teckit" = xyes:yes; then
  AC_MSG_ERROR([did not find teckit])
fi
]) # KPSE_TECKIT_SYSTEM_FLAGS
