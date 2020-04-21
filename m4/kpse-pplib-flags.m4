# Public macros for the TeX Live (TL) tree.
# Copyright (C) 2009-2015 Peter Breitenlohner <tex-live@tug.org>
#
# This file is free software; the copyright holder
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# serial 0

# KPSE_PPLIB_FLAGS
# -----------------
# Provide the configure options '--with-system-PPLIB' (if in the TL tree).
#
# Set the make variables PPLIB_INCLUDES and PPLIB_LIBS to the CPPFLAGS and
# LIBS required for the `-lpplib' library in libs/pplib/ of the TL tree.
AC_DEFUN([KPSE_PPLIB_FLAGS], [dnl
AC_REQUIRE([KPSE_SAVE_FLAGS])[]dnl
_KPSE_LIB_FLAGS([pplib], [pplib], [tree],
                [-IBLD/libs/pplib/include], [BLD/libs/pplib/libpplib.a], [],
                [], [${top_builddir}/../../libs/pplib/include/pplib.h])[]dnl
]) # KPSE_PPLIB_FLAGS

# KPSE_PPLIB_OPTIONS([WITH-SYSTEM])
# ----------------------------------
AC_DEFUN([KPSE_PPLIB_OPTIONS], [_KPSE_LIB_OPTIONS([PPLIB], [$1], [pkg-config])])

# KPSE_PPLIB_SYSTEM_FLAGS
# ------------------------
AC_DEFUN([KPSE_PPLIB_SYSTEM_FLAGS], [dnl
_KPSE_PKG_CONFIG_FLAGS([PPLIB], [PPLIB])])
